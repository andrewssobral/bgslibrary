#include "VideoCapture.h"

#if  CV_MAJOR_VERSION >= 4
//#define CV_CAP_PROP_POS_FRAMES cv::CAP_PROP_POS_FRAMES
//#define CV_CAP_PROP_FRAME_COUNT cv::CAP_PROP_FRAME_COUNT
#define CV_CAP_PROP_FPS cv::CAP_PROP_FPS
#define CV_EVENT_LBUTTONDOWN cv::EVENT_LBUTTONDOWN
#define CV_EVENT_MOUSEMOVE cv::EVENT_MOUSEMOVE
#define cvSetMouseCallback cv::setMouseCallback
#endif

namespace bgslibrary
{
  namespace VC_ROI
  {
    cv::Mat img_input1;
    cv::Mat img_input2;
    int roi_x0 = 0;
    int roi_y0 = 0;
    int roi_x1 = 0;
    int roi_y1 = 0;
    int numOfRec = 0;
    bool startDraw = false;
    bool roi_defined = false;
    bool use_roi = true;
    bool disable_event = false;

    void reset(void)
    {
      disable_event = false;
      startDraw = false;
    }

    void VideoCapture_on_mouse(int evt, int x, int y, int flag, void* param)
    {
      if (use_roi == false || disable_event == true)
        return;
      
      if (evt == CV_EVENT_LBUTTONDOWN)
      {
        if (!startDraw)
        {
          roi_x0 = x;
          roi_y0 = y;
          startDraw = 1;
        }
        else
        {
          roi_x1 = x;
          roi_y1 = y;
          startDraw = 0;
          roi_defined = true;
          disable_event = true;
        }
      }

      if (evt == CV_EVENT_MOUSEMOVE && startDraw)
      {
        //redraw ROI selection
        img_input1.copyTo(img_input2);
        cv::Point pt1(roi_x0, roi_y0);
        cv::Point pt2(roi_x1, roi_y1);
        cv::rectangle(img_input2, pt1, pt2, cv::Scalar(255, 0, 0));
        cv::imshow("Input", img_input2);
        //startDraw = false;
        //disable_event = true;
      }
    }
  }

  VideoCapture::VideoCapture() :
    key(0), start_time(0), delta_time(0), freq(0),
    fps(0), frameNumber(0), stopAt(0), useCamera(false),
    cameraIndex(0), useVideo(false), input_resize_percent(100),
    showOutput(true), showFPS(true), enableFlip(false)
  {
    debug_construction(VideoCapture);
    initLoadSaveConfig(quote(VideoCapture));
  }

  VideoCapture::~VideoCapture() {
    debug_destruction(VideoCapture);
  }

  void VideoCapture::setFrameProcessor(const std::shared_ptr<IFrameProcessor> &_frameProcessor)
  {
    frameProcessor = _frameProcessor;
  }

  void VideoCapture::setCamera(int _index)
  {
    useCamera = true;
    cameraIndex = _index;
    useVideo = false;
  }

  void VideoCapture::setUpCamera()
  {
    std::cout << "Camera index:" << cameraIndex << std::endl;
    capture.open(cameraIndex);

    if (!capture.isOpened())
      std::cerr << "Cannot initialize webcam!\n" << std::endl;
  }

  void VideoCapture::setVideo(std::string _filename)
  {
    useVideo = true;
    videoFileName = _filename;
    useCamera = false;
  }

  void VideoCapture::setUpVideo()
  {
    std::cout << "Openning: " << videoFileName << std::endl;
    capture.open(videoFileName.c_str());

    if (!capture.isOpened())
      std::cerr << "Cannot open video file " << videoFileName << std::endl;
    else
      std::cout << "OK" << std::endl;
  }

  void VideoCapture::start()
  {
    if (useCamera) setUpCamera();
    if (useVideo)  setUpVideo();
    //if (!capture)  std::cerr << "Capture error..." << std::endl;

    //using namespace std::chrono_literals;
    do
    {
      capture >> frame;
      if (frame.empty())
      {
        std::cout << "Frame is not ready" << std::endl;
        std::string dummy;
        std::cout << "Enter to continue..." << std::endl;
        std::getline(std::cin, dummy);
        //cv::waitKey(1000);
        //std::this_thread::sleep_for(1s);
      }
      else
        break;
    } while (1);

    int input_fps = capture.get(CV_CAP_PROP_FPS);
    std::cout << "input->fps:" << input_fps << std::endl;
    std::cout << "input->width:" << frame.size().width << std::endl;
    std::cout << "input->height:" << frame.size().height << std::endl;

    if (input_fps > 0)
      loopDelay = (1. / input_fps)*1000.;
    std::cout << "loopDelay:" << loopDelay << std::endl;

    std::cout << "Press 'ESC' to stop..." << std::endl;
    do
    {
      frameNumber++;

      capture >> frame;
      if (frame.empty()) break;

      cv::resize(frame, frame, cv::Size(), input_resize_percent/100., input_resize_percent / 100.);

      if (firstTime && input_resize_percent != 100)
      {
        std::cout << "Resized to:" << std::endl;
        std::cout << "input->width:" << frame.size().width << std::endl;
        std::cout << "input->height:" << frame.size().height << std::endl;
      }
      
      //if (enableFlip)
      //  cvFlip(frame, frame, 0);

      if (VC_ROI::use_roi == true && VC_ROI::roi_defined == false && firstTime == true)
      {
        VC_ROI::reset();

        do
        {
          cv::Mat img_input;
          frame.copyTo(img_input);

          if (showOutput)
          {
            cv::imshow("Input", img_input);

            std::cout << "Set ROI (press ESC to skip)" << std::endl;
            //VC_ROI::img_input1 = new IplImage(img_input);
            cvSetMouseCallback("Input", VC_ROI::VideoCapture_on_mouse, NULL);
            key = cv::waitKey(0);
            //delete VC_ROI::img_input1;
          }
          else
            key = KEY_ESC;

          if (key == KEY_ESC)
          {
            std::cout << "ROI disabled" << std::endl;
            VC_ROI::reset();
            VC_ROI::use_roi = false;
            break;
          }

          if (VC_ROI::roi_defined)
          {
            std::cout << "ROI defined (" << VC_ROI::roi_x0 << "," << VC_ROI::roi_y0 << "," << VC_ROI::roi_x1 << "," << VC_ROI::roi_y1 << ")" << std::endl;
            break;
          }
          else
            std::cout << "ROI undefined" << std::endl;

        } while (1);
      }

      if (VC_ROI::use_roi == true && VC_ROI::roi_defined == true)
      {
        cv::Rect roi(VC_ROI::roi_x0, VC_ROI::roi_y0, VC_ROI::roi_x1 - VC_ROI::roi_x0, VC_ROI::roi_y1 - VC_ROI::roi_y0);
        frame = frame(roi);
      }

      cv::Mat img_input;
      frame.copyTo(img_input);

      start_time = cv::getTickCount();
      frameProcessor->process(img_input);
      delta_time = cv::getTickCount() - start_time;
      freq = cv::getTickFrequency();
      fps = freq / delta_time;
      //std::cout << "FPS: " << fps << std::endl;
      
      if (showFPS)
        cv::putText(img_input,
              "FPS: " + std::to_string(fps),
              cv::Point(10,15), // Coordinates
              cv::FONT_HERSHEY_COMPLEX_SMALL, // Font
              1.0, // Scale. 2.0 = 2x bigger
              cv::Scalar(0,0,255), // BGR Color
              1); // Line Thickness (Optional)

      if (showOutput)
        cv::imshow("Input", img_input);

      //cvResetImageROI(frame);

      key = cv::waitKey(loopDelay);
      //std::cout << "key: " << key << std::endl;

      if (key == KEY_SPACE)
        key = cv::waitKey(0);

      if (key == KEY_ESC)
        break;

      if (stopAt > 0 && stopAt == frameNumber)
        key = cv::waitKey(0);

      firstTime = false;
    } while (1);

    capture.release();
  }

  void VideoCapture::save_config(cv::FileStorage &fs) {
    fs << "stopAt" << stopAt;
    fs << "input_resize_percent" << input_resize_percent;
    fs << "enableFlip" << enableFlip;
    fs << "use_roi" << VC_ROI::use_roi;
    fs << "roi_defined" << VC_ROI::roi_defined;
    fs << "roi_x0" << VC_ROI::roi_x0;
    fs << "roi_y0" << VC_ROI::roi_y0;
    fs << "roi_x1" << VC_ROI::roi_x1;
    fs << "roi_y1" << VC_ROI::roi_y1;
    fs << "showFPS" << showFPS;
    fs << "showOutput" << showOutput;
  }

  void VideoCapture::load_config(cv::FileStorage &fs) {
    fs["stopAt"] >> stopAt;
    fs["input_resize_percent"] >> input_resize_percent;
    fs["enableFlip"] >> enableFlip;
    fs["use_roi"] >> VC_ROI::use_roi;
    fs["roi_defined"] >> VC_ROI::roi_defined;
    fs["roi_x0"] >> VC_ROI::roi_x0;
    fs["roi_y0"] >> VC_ROI::roi_y0;
    fs["roi_x1"] >> VC_ROI::roi_x1;
    fs["roi_y1"] >> VC_ROI::roi_y1;
    fs["showFPS"] >> showFPS;
    fs["showOutput"] >> showOutput;
  }
}
