#pragma once

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

#include "src/bgslibrary_BgsLib.h"
#include "../../bgslibrary/algorithms/algorithms.h"

bool constructObject(std::string algorithm);
void computeForegroundMask(const cv::Mat &img_input, cv::Mat &img_output);
void destroyObject();

template <typename T>
std::string ToString(T val)
{
  std::stringstream stream;
  stream << val;
  return stream.str();
}

jstring StringToJString(JNIEnv * env, const std::string & nativeString)
{
  return env->NewStringUTF(nativeString.c_str());
}

void GetJStringContent(JNIEnv *AEnv, jstring AStr, std::string &ARes)
{
  if (!AStr)
  {
    ARes.clear();
    return;
  }

  const auto *s = AEnv->GetStringUTFChars(AStr, nullptr);
  ARes = s;
  AEnv->ReleaseStringUTFChars(AStr, s);
}
