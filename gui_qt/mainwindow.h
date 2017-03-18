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

#include "qt_utils.h"
#include "texteditor.h"
#include "../package_bgs/bgslibrary.h"

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
  explicit MainWindow(QWidget *parent = 0);
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
