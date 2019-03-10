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
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>

#include "package_bgs/bgslibrary.h"

int main(int argc, char **argv)
{
  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

  cv::VideoCapture capture;

  if (argc > 1)
  {
    std::cout << "Openning: " << argv[1] << std::endl;
    capture.open(argv[1]);
  }
  else
    capture.open(0);

  if (!capture.isOpened())
  {
    std::cerr << "Cannot initialize video!" << std::endl;
    return -1;
  }

  /* Background Subtraction Methods */
  auto algorithmsName = BGS_Factory::Instance()->GetRegisteredAlgorithmsName();

  auto key = 0;
  for (const std::string& algorithmName : algorithmsName)
  {
    std::cout << "Running " << algorithmName << std::endl;
    auto bgs = BGS_Factory::Instance()->Create(algorithmName);

    cv::Mat img_input;
    
    capture.set(CV_CAP_PROP_POS_FRAMES, 0); // Set index to 0 (start frame)
    auto frame_counter = 0;
    std::cout << "Press 's' to stop:" << std::endl;
    while (key != 's')
    {
      // Capture frame-by-frame
      capture >> img_input;
      frame_counter += 1;

      if (img_input.empty()) break;

      cv::imshow("input", img_input);

      cv::Mat img_mask;
      cv::Mat img_bkgmodel;
      try
      {
        bgs->process(img_input, img_mask, img_bkgmodel); // by default, it shows automatically the foreground mask image

        //if(!img_mask.empty())
        //  cv::imshow("Foreground", img_mask);
        //  do something
      }
      catch (std::exception& e)
      {
        std::cout << "Exception occurred" << std::endl;
        std::cout << e.what() << std::endl;
      }

      key = cv::waitKey(33);
    }

    std::cout << "Press 'q' to exit, or anything else to move to the next algorithm:" << std::endl;
    key = cv::waitKey(0);
    if (key == 'q')
      break;

    cv::destroyAllWindows();
  }

  capture.release();

  return 0;
}
