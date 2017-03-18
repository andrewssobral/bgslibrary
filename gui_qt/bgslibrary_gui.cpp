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
#include "mainwindow.h"

int main(int argc, char *argv[])
{
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "Background Subtraction Library v2.0.0       " << std::endl;
  std::cout << "https://github.com/andrewssobral/bgslibrary " << std::endl;
  std::cout << "by:                                         " << std::endl;
  std::cout << "Andrews Sobral (andrewssobral@gmail.com)    " << std::endl;
  std::cout << "--------------------------------------------" << std::endl;
  std::cout << "Using OpenCV version " << CV_VERSION << std::endl;

  QApplication a(argc, argv);

  QCoreApplication::setApplicationName("BGSLibrary");
  QCoreApplication::setApplicationVersion("2.0.0");

  MainWindow w;
  w.show();

  return a.exec();
}
