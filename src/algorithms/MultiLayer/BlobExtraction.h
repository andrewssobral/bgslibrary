#pragma once

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
