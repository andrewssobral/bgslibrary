#
# HOW TO COMPILE ON LINUX
#
# Requirements:
# cmake >= 2.8
# opencv >= 2.3.1

cd build
cmake ..
make
cd ..

chmod +x run_video.sh run_camera.sh run_demo.sh
./run_video.sh
./run_camera.sh
./run_demo.sh
