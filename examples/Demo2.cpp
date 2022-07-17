#include <iostream>
#include <algorithm>
#include <iterator>
#include <vector>
#include <opencv2/opencv.hpp>

#include "../src/algorithms/algorithms.h"

#if CV_MAJOR_VERSION >= 4
#define CV_LOAD_IMAGE_COLOR cv::IMREAD_COLOR
#endif

int main(int argc, char **argv)
{
  std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION << std::endl;

  std::string baseDir = "./dataset/frames";
  if (argc > 1)
    baseDir = argv[1];
  std::cout << "Openning: " << baseDir << std::endl;

  /* Background Subtraction Methods */
  auto algorithmsName = BGS_Factory::Instance()->GetRegisteredAlgorithmsName();
  
  std::cout << "List of available algorithms:" << std::endl;
  std::copy(algorithmsName.begin(), algorithmsName.end(), std::ostream_iterator<std::string>(std::cout, "\n"));
  
  for (const std::string& algorithmName : algorithmsName)
  {
    std::cout << "Running " << algorithmName << std::endl;
    auto bgs = BGS_Factory::Instance()->Create(algorithmName);

    auto frame_counter = 0;
    std::cout << "Press 's' to stop:" << std::endl;
    auto key = 0;
    while (key != 's')
    {
      // Capture frame-by-frame
      frame_counter++;
      std::stringstream ss;
      ss << frame_counter;
      auto fileName = baseDir + "/" + ss.str() + ".png";
      std::cout << "reading " << fileName << std::endl;

      auto img_input = cv::imread(fileName, CV_LOAD_IMAGE_COLOR);

      if (img_input.empty())
        break;

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
        break;
      }

      key = cv::waitKey(33);
    }

    std::cout << "Press 'q' to exit, or anything else to move to the next algorithm:" << std::endl;
    key = cv::waitKey(0);
    if (key == 'q')
      break;

    cv::destroyAllWindows();
  }

  return 0;
}
