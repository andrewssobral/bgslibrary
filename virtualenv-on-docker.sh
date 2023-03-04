# For Linux users
#
# OpenCV 4.6.0
# https://github.com/andrewssobral/docker/blob/master/bgslibrary/opencv_4.6.0py38/Dockerfile
DOCKER_IMAGE=andrewssobral/bgslibrary:opencv_4.6.0py38
#
# OpenCV 3.4.16
# https://github.com/andrewssobral/docker/blob/master/bgslibrary/opencv_3.4.16py38/Dockerfile
DOCKER_IMAGE=andrewssobral/bgslibrary:opencv_3.4.16py38
#
# OpenCV 3.4.7
# https://github.com/andrewssobral/docker/blob/master/bgslibrary/opencv_3.4.7py38/Dockerfile
DOCKER_IMAGE=andrewssobral/bgslibrary:opencv_3.4.7py38

docker run -it --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY $DOCKER_IMAGE bash

# For Mac users
# 
# X11 forwarding on macOS and docker
# https://gist.github.com/andrewssobral/7a5924c475ff936247f237f35d2e3cd7
#
# Or
#
# To run a Docker container on Mac with display support, you need to use XQuartz, a free and open-source software that provides an X11 window system for macOS.
# Here are the steps you need to follow:
# Install XQuartz: Download and install XQuartz from the official website (https://www.xquartz.org/).
# Enable XQuartz: Open XQuartz and go to XQuartz > Preferences > Security. Make sure that the “Allow connections from network clients” option is checked.
# Install socat: Install socat using Homebrew. Run the following command in the Terminal:
brew install socat
# Run socat: Run socat in the Terminal to forward X11 connections from the Docker container to XQuartz. Run the following command:
socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\"
# To check the tcp ports that are listenning
sudo lsof -i -P | grep LISTEN | grep :$PORT
# Start the Docker container: Start the Docker container with the following command:
docker run -e DISPLAY=host.docker.internal:0 <image-name>
# Replace <image-name> with the name of the Docker image you want to run.
# Test the display support: Test the display support by running a graphical application inside the Docker container. For example, you can run xclock or xeyes.
# e.g:
docker run -it --rm -e DISPLAY=host.docker.internal:0 $DOCKER_IMAGE bash
python -m pip install --upgrade pip
apt update && apt install x11-apps
xclock
xeyes
