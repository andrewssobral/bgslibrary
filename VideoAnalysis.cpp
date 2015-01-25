/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "VideoAnalysis.h"
#include <opencv2/highgui/highgui_c.h>

namespace bgslibrary
{
  VideoAnalysis::VideoAnalysis() : useVideo(false), useCamera(false), cameraId(0), frameToStop(0)
  {
    std::cout << "VideoAnalysis()" << std::endl;
  }

  VideoAnalysis::~VideoAnalysis()
  {
    std::cout << "~VideoAnalysis()" << std::endl;
  }

  bool VideoAnalysis::setup(int argc, const char **argv)
  {
    const std::string keys =
      "{h help ? |   | print this message     }"
      "{i input  |   | input video file       }"
      "{c camera | 0 | camera id              }"
      "{s stop   | 0 | stop at frame number s }"
      ;
    cv::CommandLineParser cmd(argc, argv, keys);

    if (argc <= 1 || cmd.has("help"))
    {
      std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
      std::cout << "Available options:" << std::endl;
      cmd.printMessage();
      return false;
    }

    if (!cmd.check())
    {
      cmd.printErrors();
      return false;
    }

    filename = cmd.get<std::string>("i");
    cameraId = cmd.get<int>("c");
    frameToStop = cmd.get<int>("s");

    if (!filename.empty())
      useVideo = true;
    else
      if (cameraId >= 0)
        useCamera = true;

    if (useVideo || useCamera)
      return true;
    else
      return false;
  }

  void VideoAnalysis::start()
  {
    //std::cout << "Press 'ESC' to stop..." << std::endl;

    do
    {
      videoCapture = new VideoCapture;
      frameProcessor = new FrameProcessor;

      frameProcessor->init();
      frameProcessor->frameToStop = frameToStop;

      videoCapture->setFrameProcessor(frameProcessor);

      if (useVideo)
        videoCapture->setVideo(filename);

      if (useCamera)
        videoCapture->setCamera(cameraId);

      videoCapture->start();

      if (useVideo || useCamera)
        break;

      frameProcessor->finish();

      int key = cvWaitKey(500);
      if (key == KEY_ESC)
        break;

      delete frameProcessor;
      delete videoCapture;

    } while (1);

    delete frameProcessor;
    delete videoCapture;
  }
}
