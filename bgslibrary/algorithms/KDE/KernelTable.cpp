#include <math.h>

#include "KernelTable.h"

#ifndef PI
#define PI 3.141592653589793f
#endif

using namespace bgslibrary::algorithms::kde;

KernelLUTable::KernelLUTable()
{
  std::cout << "KernelLUTable()" << std::endl;
}

KernelLUTable::~KernelLUTable()
{
  delete kerneltable;
  delete kernelsums;
  std::cout << "~KernelLUTable()" << std::endl;
}

KernelLUTable::KernelLUTable(int KernelHalfWidth, double Segmamin, double Segmamax, int Segmabins)
{
  std::cout << "KernelLUTable()" << std::endl;

  double C1, C2, v, segma, sum;
  int bin, b;

  minsegma = Segmamin;
  maxsegma = Segmamax;
  segmabins = Segmabins;
  tablehalfwidth = KernelHalfWidth;

  // Generate the Kernel

  // allocate memory for the Kernal Table
  kerneltable = new double[segmabins*(2 * KernelHalfWidth + 1)];
  kernelsums = new double[segmabins];

  double segmastep = (maxsegma - minsegma) / segmabins;
  double y;

  for (segma = minsegma, bin = 0; bin < segmabins; segma += segmastep, bin++)
  {
    C1 = 1 / (sqrt(2 * PI)*segma);
    C2 = -1 / (2 * segma*segma);

    b = (2 * KernelHalfWidth + 1)*bin;
    sum = 0;

    for (int x = 0; x <= KernelHalfWidth; x++)
    {
      y = x / 1.0;
      v = C1*exp(C2*y*y);
      kerneltable[b + KernelHalfWidth + x] = v;
      kerneltable[b + KernelHalfWidth - x] = v;
      sum += 2 * v;
    }

    sum -= C1;

    kernelsums[bin] = sum;

    // Normailization
    for (int x = 0; x <= KernelHalfWidth; x++)
    {
      v = kerneltable[b + KernelHalfWidth + x] / sum;
      kerneltable[b + KernelHalfWidth + x] = v;
      kerneltable[b + KernelHalfWidth - x] = v;
    }
  }
}
