#catkin_lint: ignore subproject
cmake_minimum_required(VERSION 3.16.3)
project(
  morb_slam_ros
  VERSION 1.0
  LANGUAGES CXX)

# Release by default Turn on Debug with "-DCMAKE_BUILD_TYPE=Debug"
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

find_package(
  catkin REQUIRED
  COMPONENTS roscpp
             rospy
             std_msgs
             cv_bridge
             image_transport
             tf
             sensor_msgs
             message_generation)
find_package(OpenCV 4.2 REQUIRED)
find_package(Eigen3 3.1.0 REQUIRED)

catkin_package(
  INCLUDE_DIRS
  include
  Thirdparty/DBoW2/include
  Thirdparty/g2o
  Thirdparty/Sophus
  LIBRARIES
  ${PROJECT_NAME}
  DBoW2
  g2o
  CATKIN_DEPENDS
  roscpp
  rospy
  std_msgs
  cv_bridge
  image_transport
  tf
  sensor_msgs
  message_runtime
  DEPENDS
  EIGEN3
  OpenCV)

add_subdirectory(Thirdparty/g2o)
add_subdirectory(Thirdparty/DBoW2)
add_subdirectory(Thirdparty/Sophus)

add_library(
  ${PROJECT_NAME}
  src/Atlas.cc
  src/CameraModels/KannalaBrandt8.cpp
  src/CameraModels/Pinhole.cpp
  src/Config.cc
  src/Converter.cc
  src/Frame.cc
  src/G2oTypes.cc
  src/GeometricTools.cc
  src/ImuTypes.cc
  src/KeyFrame.cc
  src/KeyFrameDatabase.cc
  src/LocalMapping.cc
  src/LoopClosing.cc
  src/Map.cc
  src/MapPoint.cc
  src/MLPnPsolver.cpp
  src/OptimizableTypes.cpp
  src/Optimizer.cc
  src/ORBextractor.cc
  src/ORBmatcher.cc
  src/Settings.cc
  src/Sim3Solver.cc
  src/System.cc
  src/Tracking.cc
  src/TwoViewReconstruction.cc)
target_include_directories(
  ${PROJECT_NAME}
  PUBLIC $<INSTALL_INTERFACE:include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/DBoW2/include>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/Sophus>
         $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Thirdparty/g2o>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

target_link_libraries(
  ${PROJECT_NAME}
  ${OpenCV_LIBS}
  ${EIGEN3_LIBS}
  DBoW2
  g2o
  -lboost_serialization
  -lcrypto)

target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

if(CMAKE_BUILD_TYPE EQUAL "Release" OR CMAKE_BUILD_TYPE EQUAL "RelWithDebInfo")
  target_compile_options(
    orb_slam3
    PRIVATE -O3
    PUBLIC -march=native)
endif()

add_custom_target(vocabulary ALL)
add_custom_command(
  TARGET vocabulary
  PRE_BUILD
  COMMAND ${CMAKE_COMMAND} -E tar xf ${PROJECT_SOURCE_DIR}/Vocabulary/ORBvoc.txt.tar.gz
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/Vocabulary/
  DEPENDS ${PROJECT_SOURCE_DIR}/Vocabulary/ORBvoc.txt.tar.gz
  COMMENT "Unpacking ORBvoc.txt.tar.gz"
  VERBATIM)
add_dependencies(${PROJECT_NAME} vocabulary)

# Build ROS nodes
set(LIBS
    ${EIGEN3_LIBS}
    ${catkin_LIBRARIES}
    ${OpenCV_LIBS}
    ${PROJECT_NAME})

# mono node
add_executable(${PROJECT_NAME}_mono src/ROS/mono_node.cc src/ROS/common.cc)
target_link_libraries(${PROJECT_NAME}_mono ${LIBS})
target_include_directories(
  ${PROJECT_NAME}_mono
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

# mono-inertial node
add_executable(${PROJECT_NAME}_mono_inertial src/ROS/mono_inertial_node.cc src/ROS/common.cc)
target_link_libraries(${PROJECT_NAME}_mono_inertial ${LIBS})
target_include_directories(
  ${PROJECT_NAME}_mono_inertial
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

# stereo node
add_executable(${PROJECT_NAME}_stereo src/ROS/stereo_node.cc src/ROS/common.cc)
target_link_libraries(${PROJECT_NAME}_stereo ${LIBS})
target_include_directories(
  ${PROJECT_NAME}_stereo
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

# stereo-inertial node
add_executable(${PROJECT_NAME}_stereo_inertial src/ROS/stereo_inertial_node.cc src/ROS/common.cc)
target_link_libraries(${PROJECT_NAME}_stereo_inertial ${LIBS})
target_include_directories(
  ${PROJECT_NAME}_stereo_inertial
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

# RGB-D node
add_executable(${PROJECT_NAME}_rgbd src/ROS/rgbd_node.cc src/ROS/common.cc)
target_link_libraries(${PROJECT_NAME}_rgbd ${LIBS})
target_include_directories(
  ${PROJECT_NAME}_rgbd
  PUBLIC $<INSTALL_INTERFACE:include> $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE $<BUILD_INTERFACE:${EIGEN3_INCLUDE_DIRS}> ${catkin_INCLUDE_DIRS})

# Install targets
install(
  TARGETS ${PROJECT_NAME}
          ${PROJECT_NAME}_mono
          ${PROJECT_NAME}_mono_inertial
          ${PROJECT_NAME}_stereo
          ${PROJECT_NAME}_stereo_inertial
          ${PROJECT_NAME}_rgbd
  ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION})
install(DIRECTORY include/${PROJECT_NAME}/ Thirdparty/Sophus/ DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION})
install(FILES ${PROJECT_SOURCE_DIR}/Vocabulary/ORBvoc.txt DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/Vocabulary)
