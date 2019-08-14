#pragma once

#include <iostream>

namespace bgslibrary
{
  namespace algorithms
  {
    namespace kde
    {
      class KernelLUTable
      {
      public:
        double minsegma;
        double maxsegma;
        int segmabins;
        int tablehalfwidth;
        double *kerneltable;
        double *kernelsums;

      public:
        KernelLUTable();
        ~KernelLUTable();

        KernelLUTable(int KernelHalfWidth, double Segmamin, double Segmamax, int Segmabins);
      };
    }
  }
}
