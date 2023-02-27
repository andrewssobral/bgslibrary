#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      namespace blob
      {
        //! Extreu els blobs d'una imatge
        bool BlobAnalysis(IplImage* inputImage, uchar threshold, IplImage* maskImage,
          bool borderColor, bool findmoments, blob_vector &RegionData);

        // FUNCIONS AUXILIARS

        //! Fusiona dos blobs
        void Subsume(blob_vector &RegionData, int, int*, CBlob*, CBlob*, bool, int, int);
        //! Reallocata el vector auxiliar de blobs subsumats
        int *NewSubsume(int *SubSumedRegion, int elems_inbuffer);
        //! Retorna el perimetre extern d'una run lenght
        double GetExternPerimeter(int start, int end, int row, int width, int height, IplImage *maskImage);
      }
    }
  }
}

#endif
