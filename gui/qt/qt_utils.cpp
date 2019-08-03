#include "qt_utils.h"

QImage GrayMat2QImage(cv::Mat const& src) {
  cv::Mat temp;
  src.copyTo(temp);
  QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_Indexed8);
  dest.bits();
  return dest;
}
QImage Mat2QImage(cv::Mat const& src) {
  cv::Mat temp;
  cvtColor(src, temp, CV_BGR2RGB);
  QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
  dest.bits();
  return dest;
}
cv::Mat QImage2Mat(QImage const& src) {
  cv::Mat tmp(src.height(), src.width(), CV_8UC3, (uchar*)src.bits(), src.bytesPerLine());
  cv::Mat result;
  cvtColor(tmp, result, CV_RGB2BGR);
  return result;
}

QString base64_encode(const QString string) {
  QByteArray ba;
  ba.append(string);
  return ba.toBase64();
}
QString base64_decode(const QString string) {
  QByteArray ba;
  ba.append(string);
  return QByteArray::fromBase64(ba);
}
QString md5_encode(const QString string) {
  QByteArray ba;
  ba.append(string);
  return QString(QCryptographicHash::hash((ba), QCryptographicHash::Md5).toHex());
}

int sti(const std::string &s) {
  int i;
  std::stringstream ss;
  ss << s;
  ss >> i;
  return i;
}
std::string its(int i) {
  std::stringstream ss;
  ss << i;
  return ss.str();
}

bool fileExists(QString path) {
  QFileInfo check_file(path);
  // check if file exists and if yes: Is it really a file and no directory?
  return check_file.exists() && check_file.isFile();
}
