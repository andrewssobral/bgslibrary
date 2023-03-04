# docker build --build-arg INCUBATOR_VER=$(date +%Y%m%d-%H%M%S) -t andrewssobral/bgslibrary:opencv_3.4.16py38 .
# docker run -it --rm -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=$DISPLAY andrewssobral/bgslibrary:opencv_3.4.16py38 bash
FROM andrewssobral/opencv3:3.4.16

WORKDIR /opt

# Changing INCUBATOR_VER will break the cache here
ARG INCUBATOR_VER=unknown

RUN git clone --recursive https://github.com/andrewssobral/bgslibrary.git
ENV BGSLIBRARY_HOME=$PWD/bgslibrary

# Compile C++ library
WORKDIR /opt/bgslibrary/build
RUN cmake .. && make -j4 && make install
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

# Build and install the python wrapper
WORKDIR /opt/bgslibrary
RUN python setup.py build
RUN python setup.py install
ENV PYTHONPATH=$PYTHONPATH:/opt/bgslibrary/build/lib.linux-x86_64-cpython-38

# Compile the C++ QT User Interface
WORKDIR /opt/bgslibrary/gui/qt/build
RUN cmake .. && make -j4
RUN cp bgslibrary_gui ../../..

# Compile examples
WORKDIR /opt/bgslibrary/examples/build
RUN cmake .. && make -j4

# Turn all shell scripts executable
WORKDIR /opt/bgslibrary
RUN chmod +x *.sh
