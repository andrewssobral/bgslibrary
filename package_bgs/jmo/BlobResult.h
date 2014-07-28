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
/* --- --- ---
 * Copyright (C) 2008--2010 Idiap Research Institute (.....@idiap.ch)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/************************************************************************
        BlobResult.h

        FUNCIONALITAT: Definició de la classe CBlobResult
        AUTOR: Inspecta S.L.
        MODIFICACIONS (Modificació, Autor, Data):

        FUNCTIONALITY: Definition of the CBlobResult class
        AUTHOR: Inspecta S.L.
        MODIFICATIONS (Modification, Author, Date):

        **************************************************************************/
#pragma once

#if !defined(_CLASSE_BLOBRESULT_INCLUDED)
#define _CLASSE_BLOBRESULT_INCLUDED

#include "BlobLibraryConfiguration.h"
#include <math.h>
//#include "cxcore.h"
#include <vector>
#include <functional>
#include <opencv2/core/types_c.h>
#include "blob.h"

typedef std::vector<double> double_stl_vector;

/**************************************************************************
  Filtres / Filters
  **************************************************************************/

//! accions que es poden fer amb els filtres
//! Actions performed by a filter (include or exclude blobs)
#define B_INCLUDE				1L
#define B_EXCLUDE				2L

//! condicions sobre els filtres
//! Conditions to apply the filters
#define B_EQUAL					3L
#define B_NOT_EQUAL				4L
#define B_GREATER				5L
#define B_LESS					6L
#define B_GREATER_OR_EQUAL		7L
#define B_LESS_OR_EQUAL			8L
#define B_INSIDE			    9L
#define B_OUTSIDE			    10L


/**************************************************************************
  Excepcions / Exceptions
  **************************************************************************/

//! Excepcions llençades per les funcions:
#define EXCEPTION_BLOB_OUT_OF_BOUNDS	1000
#define EXCEPCIO_CALCUL_BLOBS			1001

namespace Blob
{

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
    double_vector GetResult( funcio_calculBlob *evaluador ) const;
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

#endif // !defined(_CLASSE_BLOBRESULT_INCLUDED)

