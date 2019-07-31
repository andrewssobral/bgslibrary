#include "Amber.h"

using namespace bgslibrary::algorithms;

Amber::Amber() : model(nullptr)
{
  std::cout << "Amber()" << std::endl;
  /* Initialization of the Amber model */
  model = libamberModelNew();
  setup("./config/Amber.xml");
}

Amber::~Amber()
{
  std::cout << "~Amber()" << std::endl;
  libamberModelFree(model);
}

void Amber::process(const cv::Mat &img_input, cv::Mat &img_output, cv::Mat &img_bgmodel)
{
  init(img_input, img_output, img_bgmodel);

  if (img_input.empty())
    return;

  // Start the initialization
  unsigned int width = img_input.cols;
  unsigned int height = img_input.rows;

  if (img_input.channels() != 3)
  {
    std::cout << "Only works for 3 channels images. Sorry for that" << std::endl;
    return;
  }

  //unsigned int stride = width * 3;
  unsigned char* image = static_cast<unsigned char*>(img_input.data);

  if (firstTime)
  {
    /* Create a buffer for the output image */
    //img_output = Mat(img_input.rows, img_input.cols, CV_8UC1);

    /* Sets default model values */
    // libamberModelSetNumberOfSamples(model, nbSamples);
    // libamberModelSetMatchingThreshold(model, matchingThreshold);
    // libamberModelSetMatchingNumber(model, matchingNumber);
    // libamberModelSetUpdateFactor(model, init_subsamplingFactor);
    // libamberModelPrintParameters(model);

    /* Initiliazes the Amber model */
    libamberModelAllocInit_8u_C3R(model, image, width, height);
  }

  /* Create temporary buffers */
  unsigned char* output_segmentationMap = static_cast<unsigned char*>(calloc(width * height, sizeof(unsigned char)));
  libamberGetSegmentation_8u_C3R(model, image, output_segmentationMap);

  unsigned char* oBuffer = static_cast<unsigned char*>(img_output.data);
  unsigned char* tmpSegmentationMap = output_segmentationMap;

  for (unsigned int i = 0; i < width * height; i++)
  {
    *oBuffer = *tmpSegmentationMap;

    ++oBuffer;
    ++tmpSegmentationMap;
  }

#ifndef MEX_COMPILE_FLAG
  if (showOutput)
    imshow("Amber", img_output);
#endif

  firstTime = false;
  free(output_segmentationMap);
}

void Amber::saveConfig()
{
  cv::FileStorage fs(config_xml, cv::FileStorage::WRITE);

  fs << "showOutput" << showOutput;

  fs.release();
}

void Amber::loadConfig()
{
  cv::FileStorage fs;
  fs.open(config_xml, cv::FileStorage::READ);

  fs["showOutput"] >> showOutput;

  fs.release();
}
