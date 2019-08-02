#pragma once

#include <iostream>

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
