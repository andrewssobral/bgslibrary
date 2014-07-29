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
#include "MultiLayerBGS.h"

MultiLayerBGS::MultiLayerBGS() : firstTime(true), frameNumber(0), showOutput(true), 
saveModel(false), disableDetectMode(true), disableLearning(false), detectAfter(0), bg_model_preload(""), loadDefaultParams(true)
{
  std::cout << "MultiLayerBGS()" << std::endl;
}

MultiLayerBGS::~MultiLayerBGS()
{
  finish();
  std::cout << "~MultiLayerBGS()" << std::endl;
}

void MultiLayerBGS::setStatus(Status _status)
{
  status = _status;
}

void MultiLayerBGS::finish(void)
{
  if (bg_model_preload.empty())
  {
    bg_model_preload = "./models/MultiLayerBGSModel.yml";
    saveConfig();
  }

  if (status == MLBGS_LEARN && saveModel == true)
  {
    std::cout << "MultiLayerBGS saving background model: " << bg_model_preload << std::endl;
    BGS->Save(bg_model_preload.c_str());
  }

  cvReleaseImage(&fg_img);
  cvReleaseImage(&bg_img);
  cvReleaseImage(&fg_prob_img);
  cvReleaseImage(&fg_mask_img);
  cvReleaseImage(&fg_prob_img3);
  cvReleaseImage(&merged_img);

  delete BGS;
}

void MultiLayerBGS::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  if (img_input.empty())
    return;

  loadConfig();

  CvSize img_size = cvSize(cvCeil((double)img_input.size().width), cvCeil((double)img_input.size().height));

  if (firstTime)
  {
    if (disableDetectMode)
      status = MLBGS_LEARN;

    if (status == MLBGS_LEARN)
      std::cout << "MultiLayerBGS in LEARN mode" << std::endl;

    if (status == MLBGS_DETECT)
      std::cout << "MultiLayerBGS in DETECT mode" << std::endl;

    org_img = new IplImage(img_input);

    fg_img = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    bg_img = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    fg_prob_img = cvCreateImage(img_size, org_img->depth, 1);
    fg_mask_img = cvCreateImage(img_size, org_img->depth, 1);
    fg_prob_img3 = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    merged_img = cvCreateImage(cvSize(img_size.width * 2, img_size.height * 2), org_img->depth, org_img->nChannels);

    BGS = new CMultiLayerBGS();
    BGS->Init(img_size.width, img_size.height);
    BGS->SetForegroundMaskImage(fg_mask_img);
    BGS->SetForegroundProbImage(fg_prob_img);

    if (bg_model_preload.empty() == false)
    {
      std::cout << "MultiLayerBGS loading background model: " << bg_model_preload << std::endl;
      BGS->Load(bg_model_preload.c_str());
    }

    if (status == MLBGS_DETECT)
    {
      BGS->m_disableLearning = disableLearning;

      if (disableLearning)
        std::cout << "MultiLayerBGS disabled learning in DETECT mode" << std::endl;
      else
        std::cout << "MultiLayerBGS enabled learning in DETECT mode" << std::endl;
    }

    if (loadDefaultParams)
    {
      std::cout << "MultiLayerBGS loading default params" << std::endl;

      max_mode_num = 5;
      weight_updating_constant = 5.0;
      texture_weight = 0.5;
      bg_mode_percent = 0.6f;
      pattern_neig_half_size = 4;
      pattern_neig_gaus_sigma = 3.0f;
      bg_prob_threshold = 0.2f;
      bg_prob_updating_threshold = 0.2f;
      robust_LBP_constant = 3;
      min_noised_angle = 10.0 / 180.0 * PI; //0,01768
      shadow_rate = 0.6f;
      highlight_rate = 1.2f;
      bilater_filter_sigma_s = 3.0f;
      bilater_filter_sigma_r = 0.1f;
    }
    else
      std::cout << "MultiLayerBGS loading config params" << std::endl;

    BGS->m_nMaxLBPModeNum = max_mode_num;
    BGS->m_fWeightUpdatingConstant = weight_updating_constant;
    BGS->m_fTextureWeight = texture_weight;
    BGS->m_fBackgroundModelPercent = bg_mode_percent;
    BGS->m_nPatternDistSmoothNeigHalfSize = pattern_neig_half_size;
    BGS->m_fPatternDistConvGaussianSigma = pattern_neig_gaus_sigma;
    BGS->m_fPatternColorDistBgThreshold = bg_prob_threshold;
    BGS->m_fPatternColorDistBgUpdatedThreshold = bg_prob_updating_threshold;
    BGS->m_fRobustColorOffset = robust_LBP_constant;
    BGS->m_fMinNoisedAngle = min_noised_angle;
    BGS->m_fRobustShadowRate = shadow_rate;
    BGS->m_fRobustHighlightRate = highlight_rate;
    BGS->m_fSigmaS = bilater_filter_sigma_s;
    BGS->m_fSigmaR = bilater_filter_sigma_r;

    if (loadDefaultParams)
    {
      //frame_duration = 1.0 / 30.0;
      //frame_duration = 1.0 / 25.0;
      frame_duration = 1.0f / 10.0f;
    }

    BGS->SetFrameRate(frame_duration);

    if (status == MLBGS_LEARN)
    {
      if (loadDefaultParams)
      {
        mode_learn_rate_per_second = 0.5;
        weight_learn_rate_per_second = 0.5;
        init_mode_weight = 0.05f;
      }
      else
      {
        mode_learn_rate_per_second = learn_mode_learn_rate_per_second;
        weight_learn_rate_per_second = learn_weight_learn_rate_per_second;
        init_mode_weight = learn_init_mode_weight;
      }
    }

    if (status == MLBGS_DETECT)
    {
      if (loadDefaultParams)
      {
        mode_learn_rate_per_second = 0.01f;
        weight_learn_rate_per_second = 0.01f;
        init_mode_weight = 0.001f;
      }
      else
      {
        mode_learn_rate_per_second = detect_mode_learn_rate_per_second;
        weight_learn_rate_per_second = detect_weight_learn_rate_per_second;
        init_mode_weight = detect_init_mode_weight;
      }
    }

    BGS->SetParameters(max_mode_num, mode_learn_rate_per_second, weight_learn_rate_per_second, init_mode_weight);

    saveConfig();

    delete org_img;
  }

  //IplImage* inputImage = new IplImage(img_input);
  //IplImage* img = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
  //cvCopy(inputImage, img);
  //delete inputImage;

  if (detectAfter > 0 && detectAfter == frameNumber)
  {
    std::cout << "MultiLayerBGS in DETECT mode" << std::endl;

    status = MLBGS_DETECT;

    mode_learn_rate_per_second = 0.01f;
    weight_learn_rate_per_second = 0.01f;
    init_mode_weight = 0.001f;

    BGS->SetParameters(max_mode_num, mode_learn_rate_per_second, weight_learn_rate_per_second, init_mode_weight);

    BGS->m_disableLearning = disableLearning;

    if (disableLearning)
      std::cout << "MultiLayerBGS disabled learning in DETECT mode" << std::endl;
    else
      std::cout << "MultiLayerBGS enabled learning in DETECT mode" << std::endl;
  }

  IplImage* img = new IplImage(img_input);

  BGS->SetRGBInputImage(img);
  BGS->Process();

  BGS->GetBackgroundImage(bg_img);
  BGS->GetForegroundImage(fg_img);
  BGS->GetForegroundProbabilityImage(fg_prob_img3);
  BGS->GetForegroundMaskImage(fg_mask_img);
  BGS->MergeImages(4, img, bg_img, fg_prob_img3, fg_img, merged_img);

  img_merged = cv::Mat(merged_img);
  img_foreground = cv::Mat(fg_mask_img);
  img_background = cv::Mat(bg_img);

  if (showOutput)
  {
    cv::imshow("MLBGS Layers", img_merged);
    cv::imshow("MLBGS FG Mask", img_foreground);
  }

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);

  delete img;
  //cvReleaseImage(&img);

  firstTime = false;
  frameNumber++;
}

void MultiLayerBGS::saveConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/MultiLayerBGS.xml", 0, CV_STORAGE_WRITE);

  cvWriteString(fs, "preloadModel", bg_model_preload.c_str());
  cvWriteInt(fs, "saveModel", saveModel);
  cvWriteInt(fs, "detectAfter", detectAfter);
  cvWriteInt(fs, "disableDetectMode", disableDetectMode);
  cvWriteInt(fs, "disableLearningInDetecMode", disableLearning);
  cvWriteInt(fs, "loadDefaultParams", loadDefaultParams);

  cvWriteInt(fs, "max_mode_num", max_mode_num);
  cvWriteReal(fs, "weight_updating_constant", weight_updating_constant);
  cvWriteReal(fs, "texture_weight", texture_weight);
  cvWriteReal(fs, "bg_mode_percent", bg_mode_percent);
  cvWriteInt(fs, "pattern_neig_half_size", pattern_neig_half_size);
  cvWriteReal(fs, "pattern_neig_gaus_sigma", pattern_neig_gaus_sigma);
  cvWriteReal(fs, "bg_prob_threshold", bg_prob_threshold);
  cvWriteReal(fs, "bg_prob_updating_threshold", bg_prob_updating_threshold);
  cvWriteInt(fs, "robust_LBP_constant", robust_LBP_constant);
  cvWriteReal(fs, "min_noised_angle", min_noised_angle);
  cvWriteReal(fs, "shadow_rate", shadow_rate);
  cvWriteReal(fs, "highlight_rate", highlight_rate);
  cvWriteReal(fs, "bilater_filter_sigma_s", bilater_filter_sigma_s);
  cvWriteReal(fs, "bilater_filter_sigma_r", bilater_filter_sigma_r);

  cvWriteReal(fs, "frame_duration", frame_duration);

  cvWriteReal(fs, "learn_mode_learn_rate_per_second", learn_mode_learn_rate_per_second);
  cvWriteReal(fs, "learn_weight_learn_rate_per_second", learn_weight_learn_rate_per_second);
  cvWriteReal(fs, "learn_init_mode_weight", learn_init_mode_weight);

  cvWriteReal(fs, "detect_mode_learn_rate_per_second", detect_mode_learn_rate_per_second);
  cvWriteReal(fs, "detect_weight_learn_rate_per_second", detect_weight_learn_rate_per_second);
  cvWriteReal(fs, "detect_init_mode_weight", detect_init_mode_weight);

  cvWriteInt(fs, "showOutput", showOutput);

  cvReleaseFileStorage(&fs);
}

void MultiLayerBGS::loadConfig()
{
  CvFileStorage* fs = cvOpenFileStorage("./config/MultiLayerBGS.xml", 0, CV_STORAGE_READ);

  bg_model_preload = cvReadStringByName(fs, 0, "preloadModel", "");
  saveModel = cvReadIntByName(fs, 0, "saveModel", false);
  detectAfter = cvReadIntByName(fs, 0, "detectAfter", 0);
  disableDetectMode = cvReadIntByName(fs, 0, "disableDetectMode", true);
  disableLearning = cvReadIntByName(fs, 0, "disableLearningInDetecMode", false);
  loadDefaultParams = cvReadIntByName(fs, 0, "loadDefaultParams", true);

  max_mode_num = cvReadIntByName(fs, 0, "max_mode_num", 5);
  weight_updating_constant = cvReadRealByName(fs, 0, "weight_updating_constant", 5.0);
  texture_weight = cvReadRealByName(fs, 0, "texture_weight", 0.5);
  bg_mode_percent = cvReadRealByName(fs, 0, "bg_mode_percent", 0.6);
  pattern_neig_half_size = cvReadIntByName(fs, 0, "pattern_neig_half_size", 4);
  pattern_neig_gaus_sigma = cvReadRealByName(fs, 0, "pattern_neig_gaus_sigma", 3.0);
  bg_prob_threshold = cvReadRealByName(fs, 0, "bg_prob_threshold", 0.2);
  bg_prob_updating_threshold = cvReadRealByName(fs, 0, "bg_prob_updating_threshold", 0.2);
  robust_LBP_constant = cvReadIntByName(fs, 0, "robust_LBP_constant", 3);
  min_noised_angle = cvReadRealByName(fs, 0, "min_noised_angle", 0.01768);
  shadow_rate = cvReadRealByName(fs, 0, "shadow_rate", 0.6);
  highlight_rate = cvReadRealByName(fs, 0, "highlight_rate", 1.2);
  bilater_filter_sigma_s = cvReadRealByName(fs, 0, "bilater_filter_sigma_s", 3.0);
  bilater_filter_sigma_r = cvReadRealByName(fs, 0, "bilater_filter_sigma_r", 0.1);

  frame_duration = cvReadRealByName(fs, 0, "frame_duration", 0.1);

  learn_mode_learn_rate_per_second = cvReadRealByName(fs, 0, "learn_mode_learn_rate_per_second", 0.5);
  learn_weight_learn_rate_per_second = cvReadRealByName(fs, 0, "learn_weight_learn_rate_per_second", 0.5);
  learn_init_mode_weight = cvReadRealByName(fs, 0, "learn_init_mode_weight", 0.05);

  detect_mode_learn_rate_per_second = cvReadRealByName(fs, 0, "detect_mode_learn_rate_per_second", 0.01);
  detect_weight_learn_rate_per_second = cvReadRealByName(fs, 0, "detect_weight_learn_rate_per_second", 0.01);
  detect_init_mode_weight = cvReadRealByName(fs, 0, "detect_init_mode_weight", 0.001);

  showOutput = cvReadIntByName(fs, 0, "showOutput", true);

  cvReleaseFileStorage(&fs);
}
