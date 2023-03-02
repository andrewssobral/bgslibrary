#include "MultiLayer.h"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

using namespace bgslibrary::algorithms;

MultiLayer::MultiLayer() :
  IBGS(quote(MultiLayer)),
  frameNumber(0), saveModel(false),
  disableDetectMode(true), disableLearning(false),
  detectAfter(0), bg_model_preload(""), loadDefaultParams(true)
{
  debug_construction(MultiLayer);
  initLoadSaveConfig(algorithmName);
}

MultiLayer::~MultiLayer() {
  debug_destruction(MultiLayer);
  finish();
}

void MultiLayer::setStatus(Status _status) {
  status = _status;
}

void MultiLayer::finish() {
  if (bg_model_preload.empty()) {
    bg_model_preload = "./" + algorithmName + ".yml";
  }

  if (status == MLBGS_LEARN && saveModel == true) {
    std::cout << algorithmName + " saving background model: " << bg_model_preload << std::endl;
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

void MultiLayer::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);
  CvSize img_size = cvSize(cvCeil((double)img_input.size().width), cvCeil((double)img_input.size().height));

  if (firstTime) {
    if (disableDetectMode)
      status = MLBGS_LEARN;

    if (status == MLBGS_LEARN)
      std::cout << algorithmName + " in LEARN mode" << std::endl;

    if (status == MLBGS_DETECT)
      std::cout << algorithmName + " in DETECT mode" << std::endl;
    
    IplImage _frame = cvIplImage(img_input);
    org_img = cvCloneImage(&_frame);

    fg_img = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    bg_img = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    fg_prob_img = cvCreateImage(img_size, org_img->depth, 1);
    fg_mask_img = cvCreateImage(img_size, org_img->depth, 1);
    fg_prob_img3 = cvCreateImage(img_size, org_img->depth, org_img->nChannels);
    merged_img = cvCreateImage(cvSize(img_size.width * 2, img_size.height * 2), org_img->depth, org_img->nChannels);

    BGS = new multilayer::CMultiLayerBGS();
    BGS->Init(img_size.width, img_size.height);
    BGS->SetForegroundMaskImage(fg_mask_img);
    BGS->SetForegroundProbImage(fg_prob_img);

    if (bg_model_preload.empty() == false) {
      std::cout << algorithmName + " loading background model: " << bg_model_preload << std::endl;
      BGS->Load(bg_model_preload.c_str());
    }

    if (status == MLBGS_DETECT) {
      BGS->m_disableLearning = disableLearning;

      if (disableLearning)
        std::cout << algorithmName + " disabled learning in DETECT mode" << std::endl;
      else
        std::cout << algorithmName + " enabled learning in DETECT mode" << std::endl;
    }

    if (loadDefaultParams) {
      std::cout << algorithmName + " loading default params" << std::endl;
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
      std::cout << algorithmName + " loading config params" << std::endl;

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

    if (loadDefaultParams) {
      //frame_duration = 1.0 / 30.0;
      //frame_duration = 1.0 / 25.0;
      frame_duration = 1.0f / 10.0f;
    }

    BGS->SetFrameRate(frame_duration);

    if (status == MLBGS_LEARN) {
      if (loadDefaultParams) {
        mode_learn_rate_per_second = 0.5;
        weight_learn_rate_per_second = 0.5;
        init_mode_weight = 0.05f;
      }
      else {
        mode_learn_rate_per_second = learn_mode_learn_rate_per_second;
        weight_learn_rate_per_second = learn_weight_learn_rate_per_second;
        init_mode_weight = learn_init_mode_weight;
      }
    }

    if (status == MLBGS_DETECT) {
      if (loadDefaultParams) {
        mode_learn_rate_per_second = 0.01f;
        weight_learn_rate_per_second = 0.01f;
        init_mode_weight = 0.001f;
      }
      else {
        mode_learn_rate_per_second = detect_mode_learn_rate_per_second;
        weight_learn_rate_per_second = detect_weight_learn_rate_per_second;
        init_mode_weight = detect_init_mode_weight;
      }
    }

    BGS->SetParameters(max_mode_num, mode_learn_rate_per_second, weight_learn_rate_per_second, init_mode_weight);
  }

  //IplImage* inputImage = new IplImage(img_input);
  //IplImage* img = cvCreateImage(img_size, IPL_DEPTH_8U, 3);
  //cvCopy(inputImage, img);
  //delete inputImage;

  if (detectAfter > 0 && detectAfter == frameNumber) {
    std::cout << algorithmName + " in DETECT mode" << std::endl;
    status = MLBGS_DETECT;

    mode_learn_rate_per_second = 0.01f;
    weight_learn_rate_per_second = 0.01f;
    init_mode_weight = 0.001f;

    BGS->SetParameters(max_mode_num, mode_learn_rate_per_second, weight_learn_rate_per_second, init_mode_weight);
    BGS->m_disableLearning = disableLearning;

    if (disableLearning)
      std::cout << algorithmName + " disabled learning in DETECT mode" << std::endl;
    else
      std::cout << algorithmName + " enabled learning in DETECT mode" << std::endl;
  }

  IplImage _frame = cvIplImage(img_input);
  IplImage* img = cvCloneImage(&_frame);

  BGS->SetRGBInputImage(img);
  BGS->Process();

  BGS->GetBackgroundImage(bg_img);
  BGS->GetForegroundImage(fg_img);
  BGS->GetForegroundProbabilityImage(fg_prob_img3);
  BGS->GetForegroundMaskImage(fg_mask_img);
  BGS->MergeImages(4, img, bg_img, fg_prob_img3, fg_img, merged_img);

  img_merged = cv::cvarrToMat(merged_img);
  img_foreground = cv::cvarrToMat(fg_mask_img);
  img_background = cv::cvarrToMat(bg_img);

#ifndef MEX_COMPILE_FLAG
  if (showOutput) {
    cv::imshow(algorithmName + "_LAYERS", img_merged);
    cv::imshow(algorithmName + "_FG", img_foreground);
  }
#endif

  img_foreground.copyTo(img_output);
  img_background.copyTo(img_bgmodel);
  cvReleaseImage(&img);

  firstTime = false;
  frameNumber++;
}

void MultiLayer::save_config(cv::FileStorage &fs) {
  fs << "preloadModel" << bg_model_preload;
  fs << "saveModel" << saveModel;
  fs << "detectAfter" << detectAfter;
  fs << "disableDetectMode" << showOutput;
  fs << "disableLearningInDetecMode" << disableLearning;
  fs << "loadDefaultParams" << loadDefaultParams;
  fs << "frame_duration" << frame_duration;
  fs << "max_mode_num" << max_mode_num;
  fs << "weight_updating_constant" << weight_updating_constant;
  fs << "texture_weight" << texture_weight;
  fs << "bg_mode_percent" << bg_mode_percent;
  fs << "pattern_neig_half_size" << pattern_neig_half_size;
  fs << "pattern_neig_gaus_sigma" << pattern_neig_gaus_sigma;
  fs << "bg_prob_threshold" << bg_prob_threshold;
  fs << "bg_prob_updating_threshold" << bg_prob_updating_threshold;
  fs << "robust_LBP_constant" << robust_LBP_constant;
  fs << "min_noised_angle" << min_noised_angle;
  fs << "shadow_rate" << shadow_rate;
  fs << "highlight_rate" << highlight_rate;
  fs << "bilater_filter_sigma_s" << bilater_filter_sigma_s;
  fs << "bilater_filter_sigma_r" << bilater_filter_sigma_r;
  fs << "learn_mode_learn_rate_per_second" << learn_mode_learn_rate_per_second;
  fs << "learn_weight_learn_rate_per_second" << learn_weight_learn_rate_per_second;
  fs << "learn_init_mode_weight" << learn_init_mode_weight;
  fs << "detect_mode_learn_rate_per_second" << detect_mode_learn_rate_per_second;
  fs << "detect_weight_learn_rate_per_second" << detect_weight_learn_rate_per_second;
  fs << "detect_init_mode_weight" << detect_init_mode_weight;
  fs << "showOutput" << showOutput;
}

void MultiLayer::load_config(cv::FileStorage &fs) {
  fs["preloadModel"] >> bg_model_preload;
  fs["saveModel"] >> saveModel;
  fs["detectAfter"] >> detectAfter;
  fs["disableDetectMode"] >> disableDetectMode;
  fs["disableLearningInDetecMode"] >> disableLearning;
  fs["loadDefaultParams"] >> loadDefaultParams;
  fs["frame_duration"] >> frame_duration;
  fs["max_mode_num"] >> max_mode_num;
  fs["weight_updating_constant"] >> weight_updating_constant;
  fs["texture_weight"] >> texture_weight;
  fs["bg_mode_percent"] >> bg_mode_percent;
  fs["pattern_neig_half_size"] >> pattern_neig_half_size;
  fs["pattern_neig_gaus_sigma"] >> pattern_neig_gaus_sigma;
  fs["bg_prob_threshold"] >> bg_prob_threshold;
  fs["bg_prob_updating_threshold"] >> bg_prob_updating_threshold;
  fs["robust_LBP_constant"] >> robust_LBP_constant;
  fs["min_noised_angle"] >> min_noised_angle;
  fs["shadow_rate"] >> shadow_rate;
  fs["highlight_rate"] >> highlight_rate;
  fs["bilater_filter_sigma_s"] >> bilater_filter_sigma_s;
  fs["bilater_filter_sigma_r"] >> bilater_filter_sigma_r;
  fs["learn_mode_learn_rate_per_second"] >> learn_mode_learn_rate_per_second;
  fs["learn_weight_learn_rate_per_second"] >> learn_weight_learn_rate_per_second;
  fs["learn_init_mode_weight"] >> learn_init_mode_weight;
  fs["detect_mode_learn_rate_per_second"] >> detect_mode_learn_rate_per_second;
  fs["detect_weight_learn_rate_per_second"] >> detect_weight_learn_rate_per_second;
  fs["detect_init_mode_weight"] >> detect_init_mode_weight;
  fs["showOutput"] >> showOutput;
}

#endif
