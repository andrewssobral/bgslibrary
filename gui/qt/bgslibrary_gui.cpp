#include "mainwindow.h"

int main(int argc, char *argv[])
{
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "Background Subtraction Library v3.2.0       " << std::endl;
  std::cout << "https://github.com/andrewssobral/bgslibrary " << std::endl;
  std::cout << "by:                                         " << std::endl;
  std::cout << "Andrews Sobral (andrewssobral@gmail.com)    " << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "Using OpenCV version " << CV_VERSION << std::endl;

  QApplication a(argc, argv);

  QCoreApplication::setApplicationName("BGSLibrary");
  QCoreApplication::setApplicationVersion("3.2.0");

  MainWindow w;
  w.show();

  return a.exec();
}
