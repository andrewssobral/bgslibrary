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
#include "VideoCapture.h"
#include <opencv2/highgui/highgui_c.h>

namespace bgslibrary
{
  namespace VC_ROI
  {
    IplImage* img_input1 = 0;
    IplImage* img_input2 = 0;
    int roi_x0 = 0;
    int roi_y0 = 0;
    int roi_x1 = 0;
    int roi_y1 = 0;
    int numOfRec = 0;
    int startDraw = 0;
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
        img_input2 = cvCloneImage(img_input1);
        cvRectangle(img_input2, cvPoint(roi_x0, roi_y0), cvPoint(x, y), CV_RGB(255, 0, 0), 1);
        cvShowImage("Input", img_input2);
        cvReleaseImage(&img_input2);
        //startDraw = false;
        //disable_event = true;
      }
    }
  }

  VideoCapture::VideoCapture() :
    key(0), start_time(0), delta_time(0), freq(0),
    fps(0), frameNumber(0), stopAt(0), useCamera(false),
    useVideo(false), input_resize_percent(100), showOutput(true),
    enableFlip(false), cameraIndex(0)
  {
    std::cout << "VideoCapture()" << std::endl;
    setup("./config/VideoCapture.xml");
  }

  VideoCapture::~VideoCapture()
  {
    std::cout << "~VideoCapture()" << std::endl;
  }

  void VideoCapture::setFrameProcessor(IFrameProcessor* frameProcessorPtr)
  {
    frameProcessor = frameProcessorPtr;
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
            VC_ROI::img_input1 = new IplImage(img_input);
            cvSetMouseCallback("Input", VC_ROI::VideoCapture_on_mouse, NULL);
            key = cv::waitKey(0);
            delete VC_ROI::img_input1;
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

      if (showOutput)
        cv::imshow("Input", img_input);

      start_time = cv::getTickCount();
      frameProcessor->process(img_input);
      int64 delta_time = cv::getTickCount() - start_time;
      freq = cv::getTickFrequency();
      fps = freq / delta_time;
      //std::cout << "FPS: " << fps << std::endl;

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

  void VideoCapture::saveConfig()
  {
    CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), 0, CV_STORAGE_WRITE);

    cvWriteInt(fs, "stopAt", stopAt);
    cvWriteInt(fs, "input_resize_percent", input_resize_percent);
    cvWriteInt(fs, "enableFlip", enableFlip);
    cvWriteInt(fs, "use_roi", VC_ROI::use_roi);
    cvWriteInt(fs, "roi_defined", VC_ROI::roi_defined);
    cvWriteInt(fs, "roi_x0", VC_ROI::roi_x0);
    cvWriteInt(fs, "roi_y0", VC_ROI::roi_y0);
    cvWriteInt(fs, "roi_x1", VC_ROI::roi_x1);
    cvWriteInt(fs, "roi_y1", VC_ROI::roi_y1);
    cvWriteInt(fs, "showOutput", showOutput);

    cvReleaseFileStorage(&fs);
  }

  void VideoCapture::loadConfig()
  {
    CvFileStorage* fs = cvOpenFileStorage(config_xml.c_str(), nullptr, CV_STORAGE_READ);

    stopAt = cvReadIntByName(fs, nullptr, "stopAt", 0);
    input_resize_percent = cvReadIntByName(fs, nullptr, "input_resize_percent", 100);
    enableFlip = cvReadIntByName(fs, nullptr, "enableFlip", false);
    VC_ROI::use_roi = cvReadIntByName(fs, nullptr, "use_roi", true);
    VC_ROI::roi_defined = cvReadIntByName(fs, nullptr, "roi_defined", false);
    VC_ROI::roi_x0 = cvReadIntByName(fs, nullptr, "roi_x0", 0);
    VC_ROI::roi_y0 = cvReadIntByName(fs, nullptr, "roi_y0", 0);
    VC_ROI::roi_x1 = cvReadIntByName(fs, nullptr, "roi_x1", 0);
    VC_ROI::roi_y1 = cvReadIntByName(fs, nullptr, "roi_y1", 0);
    showOutput = cvReadIntByName(fs, nullptr, "showOutput", true);

    cvReleaseFileStorage(&fs);
  }
}
