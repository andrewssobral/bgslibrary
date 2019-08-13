#pragma once

#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QMainWindow>
#include <QFileDialog>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QImage>
#include <QTimer>
#include <QStringList>
#include <QStringListModel>
#include <QCollator>

#include <opencv2/opencv.hpp>
//#include <opencv2/imgproc/types_c.h>
//#include <opencv2/imgproc/imgproc_c.h>
//#include <opencv2/highgui/highgui_c.h>

#if  CV_MAJOR_VERSION >= 4
#define CV_CAP_PROP_POS_FRAMES cv::CAP_PROP_POS_FRAMES
#define CV_CAP_PROP_FRAME_COUNT cv::CAP_PROP_FRAME_COUNT
#define CV_CAP_PROP_FPS cv::CAP_PROP_FPS
#endif

#include "qt_utils.h"
#include "texteditor.h"
#include "../../src/algorithms/algorithms.h"

namespace bgslibrary
{
  template<typename T> IBGS * createInstance() { return new T; }
  typedef std::map<std::string, IBGS*(*)()> map_ibgs;

  IBGS* get_alg(std::string alg_name);
  QStringList get_algs_name();
}

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  private slots:
  void on_actionExit_triggered();

  void on_pushButton_inputdata_clicked();

  void on_pushButton_start_clicked();

  void on_pushButton_stop_clicked();

  void on_pushButton_out_in_clicked();

  void on_pushButton_out_fg_clicked();

  void on_pushButton_out_bg_clicked();

  void on_listView_algorithms_doubleClicked(const QModelIndex &index);

private:
  Ui::MainWindow *ui;
  QString fileName;
  cv::VideoCapture capture;
  bool useCamera = false;
  bool useVideo = false;
  bool useSequence = false;
  QStringList entryList;
  long long frameNumber = 0;
  long long frameNumber_aux = 0;
  bool setUpCapture();
  bool setUpCamera();
  bool setUpVideo();
  bool setUpSequence();
  QString getSelectedAlgorithmName();
  QTimer* timer;
  void startTimer();
  void stopTimer();
  void setFrameNumber(long long frameNumber);
  QImage cv2qimage(const cv::Mat &cv_frame);
  void processFrame(const cv::Mat &cv_frame);
  IBGS *bgs;
  void createBGS();
  void destroyBGS();
  double duration = 0;
  void tic();
  void toc();
  double fps();
  int capture_length = 0;
  TextEditor textEditor;

  public slots:
  void startCapture();
};
