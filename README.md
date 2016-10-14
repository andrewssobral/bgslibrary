BGSLibrary
==========
This branch is for compiling BGSLibrary with OpenCV 3.1.0

Docker Hub
----------
A pre-built image with Ubuntu 16.04 + VNC + OpenCV 3.1.0 (+ contrib) + Python 2.7 + BGSLibrary is available here:
https://hub.docker.com/r/andrewssobral/bgslibrary_opencv3/

**How to run**

*Pull the latest image:*
```
docker pull andrewssobral/bgslibrary_opencv3
```
*Start the container:*
```
docker run -it -p 5901:5901 andrewssobral/bgslibrary_opencv3 bash
```
*Start VNC:*
```
/usr/bin/vncserver :1 -geometry 1280x800 -depth 24 && tail -f /root/.vnc/*:1.log
```
*VNC password:* **password**

*Enjoy it!* ;-)

If you want to kill VNC:
```
vncserver -kill :1
```
