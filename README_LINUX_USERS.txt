#
# HOW TO COMPILE ON LINUX
#
# Requirements:
# cmake >= 2.8
# opencv >= 2.3.1

cd build
cmake ..
make
####### OPTIONAL #######
make install
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib
export LD_LIBRARY_PATH
# for debug: echo $LD_LIBRARY_PATH
# Next, copy the <<config>> folder from bgslibrary repository to your working space.
# Now you can run bgslibrary by: bgs -i video.avi
########################
cd ..
chmod +x run_video.sh run_camera.sh run_demo.sh
./run_video.sh
./run_camera.sh
./run_demo.sh
