/**
* 
* Source: https://github.com/thien94/orb_slam3_ros_wrapper
*
*/

#include "morb_slam_ros/ROS/common.h"

using namespace std;

class ImageGrabber
{
public:
    ImageGrabber(ORB_SLAM3::System* pSLAM):mpSLAM(pSLAM){}

    void GrabImage(const sensor_msgs::ImageConstPtr& msg);

    ORB_SLAM3::System* mpSLAM;
};

int main(int argc, char **argv)
{
    ros::init(argc, argv, "Mono");
    ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info);
    if (argc > 1)
    {
        ROS_WARN ("Arguments supplied via command line are ignored.");
    }

    ros::NodeHandle node_handler("~");
    std::string node_name = ros::this_node::getName();
    image_transport::ImageTransport image_transport(node_handler);

    std::string voc_file, settings_file;
    node_handler.param<std::string>("voc_file", voc_file, "file_not_set");
    node_handler.param<std::string>("settings_file", settings_file, "file_not_set");

    if (voc_file == "file_not_set" || settings_file == "file_not_set")
    {
        ROS_ERROR("Please provide voc_file and settings_file in the launch file");       
        ros::shutdown();
        return 1;
    }

    
    node_handler.param<std::string>("world_tf", world_frame_id, "map");
    node_handler.param<std::string>("camera_tf", cam_frame_id, "camera");
    bool enable_pangolin;
    node_handler.param<bool>("enable_pangolin", enable_pangolin, true);

    // Create SLAM system. It initializes all system threads and gets ready to process frames.
    sensor_type = ORB_SLAM3::CameraType::MONOCULAR;
    ORB_SLAM3::System SLAM(voc_file, settings_file, sensor_type);
    ImageGrabber igb(&SLAM);

    ros::Subscriber sub_img0 = node_handler.subscribe("left/image_raw", 1, &ImageGrabber::GrabImage, &igb);

    setup_ros_publishers(node_handler, image_transport, sensor_type);

    ros::spin(); 

    return 0;
}

void ImageGrabber::GrabImage(const sensor_msgs::ImageConstPtr& msg)
{
    // Copy the ros image message to cv::Mat.
    cv_bridge::CvImageConstPtr cv_ptr;
    try
    {
        cv_ptr = cv_bridge::toCvShare(msg);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }

    // ORB-SLAM3 runs in TrackMonocular()
    Sophus::SE3f Tcw = mpSLAM->TrackMonocular(cv_ptr->image, cv_ptr->header.stamp.toSec());
    Sophus::SE3f Twc = Tcw.inverse();

    ros::Time msg_time = msg->header.stamp;

    publish_ros_camera_pose(Twc, msg_time);
    publish_ros_tf_transform(Twc, world_frame_id, cam_frame_id, msg_time);
    
    publish_ros_tracked_mappoints(mpSLAM->GetTrackedMapPoints(), msg_time);
}