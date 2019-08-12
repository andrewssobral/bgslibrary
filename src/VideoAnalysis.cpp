#include "VideoAnalysis.h"

namespace bgslibrary
{
  VideoAnalysis::VideoAnalysis() :
    use_file(false), use_camera(false), cameraIndex(0),
    use_comp(false), frameToStop(0)
  {
    debug_construction(VideoAnalysis);
  }

  VideoAnalysis::~VideoAnalysis() {
    debug_destruction(VideoAnalysis);
  }

  bool VideoAnalysis::setup(int argc, const char **argv)
  {
    bool flag = false;

#if CV_MAJOR_VERSION == 2
    const char* keys =
      "{hp|help|false|Print this message}"
      "{uf|use_file|false|Use video file}"
      "{fn|filename||Specify video file}"
      "{uc|use_cam|false|Use camera}"
      "{ca|camera|0|Specify camera index}"
      "{co|use_comp|false|Use mask comparator}"
      "{st|stopAt|0|Frame number to stop}"
      "{im|imgref||Specify image file}"
      ;
#elif CV_MAJOR_VERSION >= 3
    const std::string keys =
      "{h help ?     |     | Print this message   }"
      "{uf use_file  |false| Use a video file     }"
      "{fn filename  |     | Specify a video file }"
      "{uc use_cam   |false| Use a webcamera      }"
      "{ca camera    | 0   | Specify camera index }"
      "{co use_comp  |false| Use mask comparator  }"
      "{st stopAt    | 0   | Frame number to stop }"
      "{im imgref    |     | Specify a image file }"
      ;
#endif

    cv::CommandLineParser cmd(argc, argv, keys);

#if CV_MAJOR_VERSION == 2
    if (argc <= 1 || cmd.get<bool>("help") == true)
    {
      std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
      std::cout << "Available options:" << std::endl;
      cmd.printParams();
      return false;
    }
#elif CV_MAJOR_VERSION >= 3
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
#endif

    use_file = cmd.get<bool>("uf"); //use_file
    filename = cmd.get<std::string>("fn"); //filename
    use_camera = cmd.get<bool>("uc"); //use_cam
    cameraIndex = cmd.get<int>("ca"); //camera
    use_comp = cmd.get<bool>("co"); //use_comp
    frameToStop = cmd.get<int>("st"); //stopAt
    imgref = cmd.get<std::string>("im"); //imgref

    std::cout << "use_file:    " << use_file << std::endl;
    std::cout << "filename:    " << filename << std::endl;
    std::cout << "use_camera:  " << use_camera << std::endl;
    std::cout << "cameraIndex: " << cameraIndex << std::endl;
    std::cout << "use_comp:    " << use_comp << std::endl;
    std::cout << "frameToStop: " << frameToStop << std::endl;
    std::cout << "imgref:      " << imgref << std::endl;
    //return false;

    if (use_file) {
      if (filename.empty()) {
        std::cout << "Specify filename" << std::endl;
        return false;
      }

      flag = true;
    }

    if (use_camera)
      flag = true;

    if (flag && use_comp) {
      if (imgref.empty()) {
        std::cout << "Specify image reference" << std::endl;
        return false;
      }
    }

    return flag;
  }

  void VideoAnalysis::start()
  {
    //std::cout << "Press 'ESC' to stop..." << std::endl;

    do {
      videoCapture = std::make_unique<VideoCapture>();
      frameProcessor = std::make_shared<FrameProcessor>();

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

      auto key = cv::waitKey(500);
      if (key == KEY_ESC)
        break;

    } while (1);
  }
}
