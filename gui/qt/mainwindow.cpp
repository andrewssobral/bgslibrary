#include "mainwindow.h"
#include "ui_mainwindow.h"

namespace bgslibrary
{
  //template<typename T> IBGS* createInstance() { return new T; }
  //typedef std::map<std::string, IBGS*(*)()> map_ibgs;

  IBGS* get_alg(std::string alg_name) {
    map_ibgs map;
    map["FrameDifference"] = &createInstance<FrameDifference>;
    map["StaticFrameDifference"] = &createInstance<StaticFrameDifference>;
    map["WeightedMovingMean"] = &createInstance<WeightedMovingMean>;
    map["WeightedMovingVariance"] = &createInstance<WeightedMovingVariance>;
#if CV_MAJOR_VERSION == 2
    map["MixtureOfGaussianV1"] = &createInstance<MixtureOfGaussianV1>; // only for OpenCV 2.x
#endif
    map["MixtureOfGaussianV2"] = &createInstance<MixtureOfGaussianV2>;
    map["AdaptiveBackgroundLearning"] = &createInstance<AdaptiveBackgroundLearning>;
    map["AdaptiveSelectiveBackgroundLearning"] = &createInstance<AdaptiveSelectiveBackgroundLearning>;
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    map["GMG"] = &createInstance<GMG>; // only for OpenCV >= 2.4.3
#endif
#if CV_MAJOR_VERSION >= 3
    map["KNN"] = &createInstance<KNN>; // only on OpenCV 3.x
#endif
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3
    map["DPAdaptiveMedian"] = &createInstance<DPAdaptiveMedian>;
    map["DPGrimsonGMM"] = &createInstance<DPGrimsonGMM>;
    map["DPZivkovicAGMM"] = &createInstance<DPZivkovicAGMM>;
    map["DPMean"] = &createInstance<DPMean>;
    map["DPWrenGA"] = &createInstance<DPWrenGA>;
    map["DPPratiMediod"] = &createInstance<DPPratiMediod>;
    map["DPEigenbackground"] = &createInstance<DPEigenbackground>;
    map["DPTexture"] = &createInstance<DPTexture>;
    map["T2FGMM_UM"] = &createInstance<T2FGMM_UM>;
    map["T2FGMM_UV"] = &createInstance<T2FGMM_UV>;
    map["T2FMRF_UM"] = &createInstance<T2FMRF_UM>;
    map["T2FMRF_UV"] = &createInstance<T2FMRF_UV>;
    map["FuzzySugenoIntegral"] = &createInstance<FuzzySugenoIntegral>;
    map["FuzzyChoquetIntegral"] = &createInstance<FuzzyChoquetIntegral>;
    map["LBSimpleGaussian"] = &createInstance<LBSimpleGaussian>;
    map["LBFuzzyGaussian"] = &createInstance<LBFuzzyGaussian>;
    map["LBMixtureOfGaussians"] = &createInstance<LBMixtureOfGaussians>;
    map["LBAdaptiveSOM"] = &createInstance<LBAdaptiveSOM>;
    map["LBFuzzyAdaptiveSOM"] = &createInstance<LBFuzzyAdaptiveSOM>;
    map["VuMeter"] = &createInstance<VuMeter>;
    map["KDE"] = &createInstance<KDE>;
    map["IndependentMultimodal"] = &createInstance<IndependentMultimodal>;
    map["MultiCue"] = &createInstance<MultiCue>;
#endif
#if (CV_MAJOR_VERSION == 2) || (CV_MAJOR_VERSION == 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7)
    map["LBP_MRF"] = &createInstance<LBP_MRF>;
    map["MultiLayer"] = &createInstance<MultiLayer>;
#endif
    map["PixelBasedAdaptiveSegmenter"] = &createInstance<PixelBasedAdaptiveSegmenter>;
    map["SigmaDelta"] = &createInstance<SigmaDelta>;
    map["SuBSENSE"] = &createInstance<SuBSENSE>;
    map["LOBSTER"] = &createInstance<LOBSTER>;
    map["PAWCS"] = &createInstance<PAWCS>;
    map["TwoPoints"] = &createInstance<TwoPoints>;
    map["ViBe"] = &createInstance<ViBe>;
    map["CodeBook"] = &createInstance<CodeBook>;

    return map[alg_name]();
  }

  QStringList get_algs_name()
  {
    QStringList stringList;
    stringList.append("FrameDifference");
    stringList.append("StaticFrameDifference");
    stringList.append("WeightedMovingMean");
    stringList.append("WeightedMovingVariance");
#if CV_MAJOR_VERSION == 2
    stringList.append("MixtureOfGaussianV1"); // only for OpenCV 2.x
#endif
    stringList.append("MixtureOfGaussianV2");
    stringList.append("AdaptiveBackgroundLearning");
    stringList.append("AdaptiveSelectiveBackgroundLearning");
#if CV_MAJOR_VERSION == 2 && CV_MINOR_VERSION >= 4 && CV_SUBMINOR_VERSION >= 3
    stringList.append("GMG"); // only for OpenCV >= 2.4.3
#endif
#if CV_MAJOR_VERSION >= 3
    stringList.append("KNN"); // only on OpenCV 3.x
#endif
#if CV_MAJOR_VERSION == 2 || CV_MAJOR_VERSION == 3
    stringList.append("DPAdaptiveMedian");
    stringList.append("DPGrimsonGMM");
    stringList.append("DPZivkovicAGMM");
    stringList.append("DPMean");
    stringList.append("DPWrenGA");
    stringList.append("DPPratiMediod");
    stringList.append("DPEigenbackground");
    stringList.append("DPTexture");
    stringList.append("T2FGMM_UM");
    stringList.append("T2FGMM_UV");
    stringList.append("T2FMRF_UM");
    stringList.append("T2FMRF_UV");
    stringList.append("FuzzySugenoIntegral");
    stringList.append("FuzzyChoquetIntegral");
    stringList.append("LBSimpleGaussian");
    stringList.append("LBFuzzyGaussian");
    stringList.append("LBMixtureOfGaussians");
    stringList.append("LBAdaptiveSOM");
    stringList.append("LBFuzzyAdaptiveSOM");
    stringList.append("VuMeter");
    stringList.append("KDE");
    stringList.append("IndependentMultimodal");
    stringList.append("MultiCue");
#endif
#if (CV_MAJOR_VERSION == 2) || (CV_MAJOR_VERSION == 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7)
    stringList.append("LBP_MRF");
    stringList.append("MultiLayer");
#endif
    stringList.append("PixelBasedAdaptiveSegmenter");
    stringList.append("SigmaDelta");
    stringList.append("SuBSENSE");
    stringList.append("LOBSTER");
    stringList.append("PAWCS");
    stringList.append("TwoPoints");
    stringList.append("ViBe");
    stringList.append("CodeBook");
    return stringList;
  }
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  //QDir applicationPath(QCoreApplication::applicationDirPath());
  fileName = QDir(".").filePath("dataset/video.avi");
  //fileName = applicationPath.absolutePath() + "dataset";
  ui->lineEdit_inputdata->setText(fileName);
  //fileName = ui->lineEdit_inputdata->text();
  timer = new QTimer(this); connect(timer, SIGNAL(timeout()), this, SLOT(startCapture()));
  QStringListModel* listModel = new QStringListModel(bgslibrary::get_algs_name(), nullptr);
  listModel->sort(0);
  ui->listView_algorithms->setModel(listModel);
  QModelIndex index = listModel->index(0);
  ui->listView_algorithms->selectionModel()->select(index, QItemSelectionModel::Select);
  //std::cout << "QDir::currentPath(): " + QDir::currentPath().toStdString() << std::endl;
  //std::cout << "QCoreApplication::applicationDirPath(): " + QCoreApplication::applicationDirPath().toStdString() << std::endl;
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::on_actionExit_triggered()
{
  this->close();
}

void MainWindow::on_pushButton_inputdata_clicked()
{
  QFileDialog dialog(this);

  if (ui->checkBox_imageseq->isChecked())
    dialog.setFileMode(QFileDialog::Directory);
  else
    dialog.setFileMode(QFileDialog::ExistingFile);
  //dialog.setFileMode(QFileDialog::AnyFile);

  dialog.exec();
  QStringList list = dialog.selectedFiles();
  /*
    for(int index = 0; index < list.length(); index++)
       std::cout << list.at(index).toStdString() << std::endl;
    */
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_inputdata->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_in_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_in->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_fg_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_fg->setText(fileName);
  }
}

void MainWindow::on_pushButton_out_bg_clicked()
{
  QFileDialog dialog(this);
  dialog.setDirectory(".");
  dialog.setFileMode(QFileDialog::Directory);
  dialog.exec();
  QStringList list = dialog.selectedFiles();
  if (list.size() > 0)
  {
    fileName = list.at(0);
    ui->lineEdit_out_bg->setText(fileName);
  }
}

void MainWindow::on_pushButton_start_clicked()
{
  useCamera = false;
  useVideo = false;
  useSequence = false;

  if (ui->checkBox_webcamera->isChecked())
    useCamera = true;
  else
  {
    if (ui->checkBox_imageseq->isChecked())
      useSequence = true;
    else
      useVideo = true;
  }

  if (!timer->isActive() && setUpCapture())
  {
    createBGS();
    startTimer();
  }
}

void MainWindow::on_pushButton_stop_clicked()
{
  stopTimer();
}

void MainWindow::createBGS()
{
  QString algorithm_name = getSelectedAlgorithmName();
  bgs = bgslibrary::get_alg(algorithm_name.toStdString());
  bgs->setShowOutput(false);
}

void MainWindow::destroyBGS()
{
  delete bgs;
}

void MainWindow::startTimer()
{
  std::cout << "startTimer()" << std::endl;

  ui->progressBar->setValue(0);
  setFrameNumber(0);
  frameNumber_aux = 0;
  timer->start(33);

  // disable options
}

void MainWindow::stopTimer()
{
  if (!timer->isActive())
    return;

  std::cout << "stopTimer()" << std::endl;

  timer->stop();
  //setFrameNumber(0);
  //ui->progressBar->setValue(0);

  destroyBGS();

  if (useCamera || useVideo)
    capture.release();

  // enable options
}

void MainWindow::setFrameNumber(long long _frameNumber)
{
  //std::cout << "setFrameNumber()" << std::endl;
  frameNumber = _frameNumber;
  QString txt_frameNumber = QString::fromStdString(to_string<long long>(frameNumber));
  ui->label_framenumber_txt->setText(txt_frameNumber);
}

bool MainWindow::setUpCapture()
{
  capture_length = 0;

  if (useCamera && !setUpCamera()) {
    std::cout << "Cannot initialize webcamera!" << std::endl;
    return false;
  }

  if (useVideo && !setUpVideo()) {
    std::cout << "Cannot open video file " << fileName.toStdString() << std::endl;
    return false;
  }

  if (useSequence && !setUpSequence()) {
    std::cout << "Cannot process images at " << fileName.toStdString() << std::endl;
    return false;
  }

  if (useCamera || useVideo) {
    int capture_fps = static_cast<int>(capture.get(CV_CAP_PROP_FPS));
    std::cout << "capture_fps: " << capture_fps << std::endl;
  }

  if (useVideo) {
    capture_length = static_cast<int>(capture.get(CV_CAP_PROP_FRAME_COUNT));
    std::cout << "capture_length: " << capture_length << std::endl;
  }

  std::cout << "OK!" << std::endl;
  return true;
}

void MainWindow::startCapture()
{
  //std::cout << "startCapture()" << std::endl;
  setFrameNumber(frameNumber + 1);
  cv::Mat cv_frame;

  if (useCamera || useVideo)
    capture >> cv_frame;

  if (useSequence && (frameNumber - 1) < entryList.length())
  {
    QString file = entryList.at(static_cast<int>(frameNumber - 1));
    QString filePath = QDir(fileName).filePath(file);

    std::cout << "Processing: " << filePath.toStdString() << std::endl;
    if (fileExists(filePath))
      cv_frame = cv::imread(filePath.toStdString());
  }

  if (cv_frame.empty())
  {
    stopTimer();
    return;
  }

  if (frameNumber == 1)
  {
    int frame_width = cv_frame.size().width;
    int frame_height = cv_frame.size().height;
    ui->label_frameresw_txt->setText(QString::fromStdString(to_string<int>(frame_width)));
    ui->label_frameresh_txt->setText(QString::fromStdString(to_string<int>(frame_height)));
  }

  if (useVideo && capture_length > 0)
  {
    double perc = (double(frameNumber) / double(capture_length)) * 100.0;
    //std::cout << "perc: " << perc << std::endl;
    ui->progressBar->setValue(static_cast<int>(perc));
  }

  int startAt = ui->spinBox_startat->value();
  if (startAt > 0 && frameNumber < startAt)
  {
    timer->setInterval(1);
    return;
  }
  else
    timer->setInterval(33);

  int stopAt = ui->spinBox_stopat->value();
  if (stopAt > 0 && frameNumber >= stopAt)
  {
    stopTimer();
    return;
  }

  cv::Mat cv_frame_small;
  cv::resize(cv_frame, cv_frame_small, cv::Size(250, 250));

  QImage qt_frame = cv2qimage(cv_frame_small);
  ui->label_img_in->setPixmap(QPixmap::fromImage(qt_frame, Qt::MonoOnly));

  processFrame(cv_frame);
}

QImage MainWindow::cv2qimage(const cv::Mat &cv_frame)
{
  if (cv_frame.channels() == 3)
    return Mat2QImage(cv_frame);
  else
    return GrayMat2QImage(cv_frame);
}

void MainWindow::processFrame(const cv::Mat &cv_frame)
{
  cv::Mat cv_fg;
  cv::Mat cv_bg;
  tic();
  bgs->process(cv_frame, cv_fg, cv_bg);
  toc();
  ui->label_fps_txt->setText(QString::fromStdString(to_string(fps())));

  cv::Mat cv_fg_small;
  cv::resize(cv_fg, cv_fg_small, cv::Size(250, 250));
  QImage qt_fg = cv2qimage(cv_fg_small);
  ui->label_img_fg->setPixmap(QPixmap::fromImage(qt_fg, Qt::MonoOnly));

  cv::Mat cv_bg_small;
  cv::resize(cv_bg, cv_bg_small, cv::Size(250, 250));
  QImage qt_bg = cv2qimage(cv_bg_small);
  ui->label_img_bg->setPixmap(QPixmap::fromImage(qt_bg, Qt::MonoOnly));

  if (ui->checkBox_save_im->isChecked() || ui->checkBox_save_fg->isChecked() || ui->checkBox_save_bg->isChecked())
  {
    if (ui->checkBox_kfn->isChecked())
      frameNumber_aux = frameNumber;
    else
      frameNumber_aux = frameNumber_aux + 1;
  }
  if (ui->checkBox_save_im->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_in->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber_aux) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_frame);
  }
  if (ui->checkBox_save_fg->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_fg->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber_aux) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_fg);
  }
  if (ui->checkBox_save_bg->isChecked())
  {
    QString out_im_path = ui->lineEdit_out_bg->text();
    QString out_im_file = QDir(out_im_path).filePath(QString::number(frameNumber) + ".png");
    cv::imwrite(out_im_file.toStdString(), cv_bg);
  }
}

void MainWindow::tic()
{
  duration = static_cast<double>(cv::getTickCount());
}

void MainWindow::toc()
{
  duration = (static_cast<double>(cv::getTickCount()) - duration) / cv::getTickFrequency();
  //std::cout << "time(sec):" << std::fixed << std::setprecision(6) << duration << std::endl;
  //std::cout << duration << std::endl;
}

double MainWindow::fps()
{
  //double fps  = frameNumber / duration;
  double fps = 1 / duration;
  //std::cout << "Estimated frames per second : " << fps << std::endl;
  return fps;
}

bool MainWindow::setUpCamera()
{
  int cameraIndex = ui->spinBox_webcamera->value();
  std::cout << "Camera index: " << cameraIndex << std::endl;

  capture.open(cameraIndex);
  return capture.isOpened();
}

bool MainWindow::setUpVideo()
{
  //std::string videoFileName = fileName.toStdString();
  std::string videoFileName = ui->lineEdit_inputdata->text().toStdString();
  std::cout << "Openning: " << videoFileName << std::endl;
  capture.open(videoFileName.c_str());
  return capture.isOpened();
}

bool MainWindow::setUpSequence()
{
  std::cout << "Directory path: " << fileName.toStdString() << std::endl;
  if (QDir(fileName).exists())
  {
    QDir dir(fileName);
    QStringList filters;
    filters << "*.png" << "*.jpg" << "*.bmp" << "*.gif";
    dir.setNameFilters(filters);
    //entryList = dir.entryList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    //entryList = dir.entryList(QDir::Filter::Files, QDir::SortFlag::NoSort);
    dir.setFilter(QDir::Files | QDir::NoSymLinks);
    dir.setSorting(QDir::NoSort);  // will sort manually with std::sort
    //dir.setSorting(QDir::LocaleAware);
    entryList = dir.entryList();

    std::cout << entryList.length() << std::endl;
    if (entryList.length() == 0)
    {
      QMessageBox::warning(this, "Warning", "No image found! (png, jpg, bmp, gif)");
      return false;
    }

    QCollator collator;
    collator.setNumericMode(true);
    std::sort(
      entryList.begin(),
      entryList.end(),
      [&collator](const QString &file1, const QString &file2)
    {
      return collator.compare(file1, file2) < 0;
    });

    //    for(int i = 0; i < entryList.length(); i++)
    //    {
    //      QString file = entryList.at(i);
    //      std::cout << file.toStdString() << std::endl;
    //    }
    return true;
  }
  else
  {
    QMessageBox::warning(this, "Warning", "Directory path doesn't exist!");
    return false;
  }
}

QString MainWindow::getSelectedAlgorithmName()
{
  QModelIndex index = ui->listView_algorithms->currentIndex();
  QString algorithm_name = index.data(Qt::DisplayRole).toString();
  return algorithm_name;
}

void MainWindow::on_listView_algorithms_doubleClicked(const QModelIndex &index)
{
  QString algorithm_name = index.data(Qt::DisplayRole).toString();
  std::cout << "Selected algorithm: " << algorithm_name.toStdString() << std::endl;

  //  CodeEditor editor;
  //  editor.setWindowTitle(QObject::tr("Code Editor Example"));
  //  editor.show();

  QString configFileName = QDir(".").filePath("config/" + algorithm_name + ".xml");
  std::cout << "Looking for: " << configFileName.toStdString() << std::endl;

  if (fileExists(configFileName))
  {
    textEditor.loadFile(configFileName);
    textEditor.show();
  }
  else
  {
    QMessageBox::warning(this, "Warning", "XML configuration file not found!\nPlease run the algorithm first or create a config folder relative to the executable path.");
    return;
  }
}
