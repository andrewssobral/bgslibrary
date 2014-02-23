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

namespace bgslibrary
{
  VideoAnalysis::VideoAnalysis() : use_file(false), use_camera(false), cameraIndex(0), use_comp(false), frameToStop(0)
  {
    std::cout << "VideoAnalysis()" << std::endl;
  }

  VideoAnalysis::~VideoAnalysis()
  {
    std::cout << "~VideoAnalysis()" << std::endl;
  }

  bool VideoAnalysis::setup(int argc, const char **argv)
  {
    bool flag = false;

    const char* keys =
      "{hp|help|false|Print help message}"
      "{uf|use_file|false|Use video file}"
      "{fn|filename||Specify video file}"
      "{uc|use_cam|false|Use camera}"
      "{ca|camera|0|Specify camera index}"
      "{co|use_comp|false|Use mask comparator}"
      "{st|stopAt|0|Frame number to stop}"
      "{im|imgref||Specify image file}"
      ;
    cv::CommandLineParser cmd(argc, argv, keys);

    if (argc <= 1 || cmd.get<bool>("help") == true)
    {
      std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
      std::cout << "Avaible options:" << std::endl;
      cmd.printParams();
      return false;
    }

    use_file = cmd.get<bool>("use_file");
    if (use_file)
    {
      filename = cmd.get<std::string>("filename");

      if (filename.empty())
      {
        std::cout << "Specify filename" << std::endl;
        return false;
      }

      flag = true;
    }

    use_camera = cmd.get<bool>("use_cam");
    if (use_camera)
    {
      cameraIndex = cmd.get<int>("camera");
      flag = true;
    }

    if (flag == true)
    {
      use_comp = cmd.get<bool>("use_comp");
      if (use_comp)
      {
        frameToStop = cmd.get<int>("stopAt");
        imgref = cmd.get<std::string>("imgref");

        if (imgref.empty())
        {
          std::cout << "Specify image reference" << std::endl;
          return false;
        }
      }
    }

    return flag;
  }

  void VideoAnalysis::start()
  {
    do
    {
      videoCapture = new VideoCapture;
      frameProcessor = new FrameProcessor;

      frameProcessor->init();
      frameProcessor->frameToStop = frameToStop;
      frameProcessor->imgref = imgref;

      videoCapture->setFrameProcessor(frameProcessor);

      if (use_file)
        videoCapture->setVideo(filename);

      if (use_camera)
        videoCapture->setCamera(cameraIndex);

      videoCapture->start();

      if (use_file || use_camera)
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
