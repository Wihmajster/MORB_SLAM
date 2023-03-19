#apt update for good measure
sudo apt update

#g20
sudo apt install -y libeigen3-dev
sudo apt install -y libsuitesparse-dev qtdeclarative5-dev qt5-qmake libqglviewer-dev-qt5
sudo apt install -y libceres-dev


folder=${PWD##*/}
folder=${result:-/}
if [ "$folder" == "MORB_SLAM" ]; then
	cd ..
fi


# morbslam
sudo apt install -y libboost-all-dev libssl-dev
git clone git@github.com:Soldann/MORB_SLAM.git
cd MORB_SLAM
chmod +x build.sh
./build.sh -j$(nproc)
