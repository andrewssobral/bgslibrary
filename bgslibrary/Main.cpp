#include <iostream>

#include "utils/GenericKeys.h"
#include "VideoAnalysis.h"

namespace bgslibrary
{
  class Main
  {
  private:
    Main();

  public:
    static void start(int argc, const char **argv)
    {
      std::cout << "---------------------------------------------" << std::endl;
      std::cout << "Background Subtraction Library               " << std::endl;
      std::cout << "https://github.com/andrewssobral/bgslibrary  " << std::endl;
      std::cout << "This software is under the MIT License       " << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      std::cout << "Using OpenCV version " << CV_VERSION << std::endl;

      try
      {
        auto key = KEY_ESC;

        do
        {
          auto videoAnalysis = std::make_unique<VideoAnalysis>();

          if (videoAnalysis->setup(argc, argv))
          {
            videoAnalysis->start();

            std::cout << "Processing finished, enter:" << std::endl;
            std::cout << "R - Repeat" << std::endl;
            std::cout << "Q - Quit" << std::endl;

            key = cv::waitKey();
          }

          cv::destroyAllWindows();

        } while (key == KEY_REPEAT);
      }
      catch (const std::exception& ex)
      {
        std::cout << "std::exception:" << ex.what() << std::endl;
        return;
      }
      catch (...)
      {
        std::cout << "Unknow error" << std::endl;
        return;
      }

#ifdef WIN32
      //system("pause");
#endif
    }
  };
}

int main(int argc, const char **argv)
{
  bgslibrary::Main::start(argc, argv);
  return 0;
}
