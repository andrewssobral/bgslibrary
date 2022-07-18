#pragma once

#include <math.h>
#include <vector>
#include <functional>

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

#include "BlobLibraryConfiguration.h"
// opencv legacy includes
#include "OpenCvLegacyIncludes.h"
#include "blob.h"

namespace bgslibrary
{
  namespace algorithms
  {
    namespace multilayer
    {
      namespace blob
      {
        typedef std::vector<double> double_stl_vector;

        /**************************************************************************
          Filtres / Filters
          **************************************************************************/

          //! accions que es poden fer amb els filtres
          //! Actions performed by a filter (include or exclude blobs)
        const long B_INCLUDE = 1L;
        const long B_EXCLUDE = 2L;

        //! condicions sobre els filtres
        //! Conditions to apply the filters
        const long B_EQUAL = 3L;
        const long B_NOT_EQUAL = 4L;
        const long B_GREATER = 5L;
        const long B_LESS = 6L;
        const long B_GREATER_OR_EQUAL = 7L;
        const long B_LESS_OR_EQUAL = 8L;
        const long B_INSIDE = 9L;
        const long B_OUTSIDE = 10L;

        /**************************************************************************
          Excepcions / Exceptions
          **************************************************************************/

          //! Excepcions llençades per les funcions:
        const int EXCEPTION_BLOB_OUT_OF_BOUNDS = 1000;
        const int EXCEPCIO_CALCUL_BLOBS = 1001;

        //! definició de que es un vector de blobs
        typedef std::vector<CBlob*>	blob_vector;

        /**
            Classe que conté un conjunt de blobs i permet extreure'n propietats
            o filtrar-los segons determinats criteris.
            Class to calculate the blobs of an image and calculate some properties
            on them. Also, the class provides functions to filter the blobs using
            some criteria.
            */
        class CBlobResult
        {
        public:
          //! constructor estandard, crea un conjunt buit de blobs
          //! Standard constructor, it creates an empty set of blobs
          CBlobResult();
          //! constructor a partir d'una imatge
          //! Image constructor, it creates an object with the blobs of the image
          CBlobResult(IplImage *source, IplImage *mask, int threshold, bool findmoments);
          //! constructor de còpia
          //! Copy constructor
          CBlobResult(const CBlobResult &source);
          //! Destructor
          virtual ~CBlobResult();

          //! operador = per a fer assignacions entre CBlobResult
          //! Assigment operator
          CBlobResult& operator=(const CBlobResult& source);
          //! operador + per concatenar dos CBlobResult
          //! Addition operator to concatenate two sets of blobs
          CBlobResult operator+(const CBlobResult& source);

          //! Afegeix un blob al conjunt
          //! Adds a blob to the set of blobs
          void AddBlob(CBlob *blob);

      #ifdef MATRIXCV_ACTIU
          //! Calcula un valor sobre tots els blobs de la classe retornant una MatrixCV
          //! Computes some property on all the blobs of the class
          double_vector GetResult(funcio_calculBlob *evaluador) const;
      #endif
          //! Calcula un valor sobre tots els blobs de la classe retornant un std::vector<double>
          //! Computes some property on all the blobs of the class
          double_stl_vector GetSTLResult(funcio_calculBlob *evaluador) const;

          //! Calcula un valor sobre un blob de la classe
          //! Computes some property on one blob of the class
          double GetNumber(int indexblob, funcio_calculBlob *evaluador) const;

          //! Retorna aquells blobs que compleixen les condicions del filtre en el destination
          //! Filters the blobs of the class using some property
          void Filter(CBlobResult &dst,
            int filterAction, funcio_calculBlob *evaluador,
            int condition, double lowLimit, double highLimit = 0);

          //! Retorna l'enèssim blob segons un determinat criteri
          //! Sorts the blobs of the class acording to some criteria and returns the n-th blob
          void GetNthBlob(funcio_calculBlob *criteri, int nBlob, CBlob &dst) const;

          //! Retorna el blob enèssim
          //! Gets the n-th blob of the class ( without sorting )
          CBlob GetBlob(int indexblob) const;
          CBlob *GetBlob(int indexblob);

          //! Elimina tots els blobs de l'objecte
          //! Clears all the blobs of the class
          void ClearBlobs();

          //! Escriu els blobs a un fitxer
          //! Prints some features of all the blobs in a file
          void PrintBlobs(char *nom_fitxer) const;


          //Metodes GET/SET

          //! Retorna el total de blobs
          //! Gets the total number of blobs
          int GetNumBlobs() const
          {
            return(m_blobs.size());
          }

        private:
          //! Funció per gestionar els errors
          //! Function to manage the errors
          void RaiseError(const int errorCode) const;

        protected:
          //! Vector amb els blobs
          //! Vector with all the blobs
          blob_vector		m_blobs;
        };
      }
    }
  }
}

#endif
