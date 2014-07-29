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
BlobResult.cpp

FUNCIONALITAT: Implementació de la classe CBlobResult
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):

**************************************************************************/

#include <limits.h>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include "BlobResult.h"
#include "BlobExtraction.h"

/**************************************************************************
Constructors / Destructors
**************************************************************************/

namespace Blob
{

  /**
  - FUNCIÓ: CBlobResult
  - FUNCIONALITAT: Constructor estandard.
  - PARÀMETRES:
  - RESULTAT:
  - Crea un CBlobResult sense cap blob
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobResult
  - FUNCTIONALITY: Standard constructor
  - PARAMETERS:
  - RESULT:
  - creates an empty set of blobs
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult::CBlobResult()
  {
    m_blobs = blob_vector();
  }

  /**
  - FUNCIÓ: CBlobResult
  - FUNCIONALITAT: Constructor a partir d'una imatge. Inicialitza la seqüència de blobs
  amb els blobs resultants de l'anàlisi de blobs de la imatge.
  - PARÀMETRES:
  - source: imatge d'on s'extreuran els blobs
  - mask: màscara a aplicar. Només es calcularan els blobs on la màscara sigui
  diferent de 0. Els blobs que toquin a un pixel 0 de la màscara seran
  considerats exteriors.
  - threshold: llindar que s'aplicarà a la imatge source abans de calcular els blobs
  - findmoments: indica si s'han de calcular els moments de cada blob
  - RESULTAT:
  - objecte CBlobResult amb els blobs de la imatge source
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlob
  - FUNCTIONALITY: Constructor from an image. Fills an object with all the blobs in
  the image
  - PARAMETERS:
  - source: image to extract the blobs from
  - mask: optional mask to apply. The blobs will be extracted where the mask is
  not 0. All the neighbouring blobs where the mask is 0 will be extern blobs
  - threshold: threshold level to apply to the image before computing blobs
  - findmoments: true to calculate the blob moments (slower)
  - RESULT:
  - object with all the blobs in the image. It throws an EXCEPCIO_CALCUL_BLOBS
  if some error appears in the BlobAnalysis function
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult::CBlobResult(IplImage *source, IplImage *mask, int threshold, bool findmoments)
  {
    bool success;

    try
    {
      // cridem la funció amb el marc a true=1=blanc (així no unirà els blobs externs)
      success = BlobAnalysis(source, (uchar)threshold, mask, true, findmoments, m_blobs);
    }
    catch (...)
    {
      success = false;
    }

    if (!success) throw EXCEPCIO_CALCUL_BLOBS;
  }

  /**
  - FUNCIÓ: CBlobResult
  - FUNCIONALITAT: Constructor de còpia. Inicialitza la seqüència de blobs
  amb els blobs del paràmetre.
  - PARÀMETRES:
  - source: objecte que es copiarà
  - RESULTAT:
  - objecte CBlobResult amb els blobs de l'objecte source
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobResult
  - FUNCTIONALITY: Copy constructor
  - PARAMETERS:
  - source: object to copy
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult::CBlobResult(const CBlobResult &source)
  {
    m_blobs = blob_vector(source.GetNumBlobs());

    // creem el nou a partir del passat com a paràmetre
    m_blobs = blob_vector(source.GetNumBlobs());
    // copiem els blobs de l'origen a l'actual
    blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
    blob_vector::iterator pBlobsDst = m_blobs.begin();

    while (pBlobsSrc != source.m_blobs.end())
    {
      // no podem cridar a l'operador = ja que blob_vector és un 
      // vector de CBlob*. Per tant, creem un blob nou a partir del
      // blob original
      *pBlobsDst = new CBlob(**pBlobsSrc);
      ++pBlobsSrc;
      ++pBlobsDst;
    }
  }



  /**
  - FUNCIÓ: ~CBlobResult
  - FUNCIONALITAT: Destructor estandard.
  - PARÀMETRES:
  - RESULTAT:
  - Allibera la memòria reservada de cadascun dels blobs de la classe
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: ~CBlobResult
  - FUNCTIONALITY: Destructor
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult::~CBlobResult()
  {
    ClearBlobs();
  }

  /**************************************************************************
  Operadors / Operators
  **************************************************************************/


  /**
  - FUNCIÓ: operador =
  - FUNCIONALITAT: Assigna un objecte source a l'actual
  - PARÀMETRES:
  - source: objecte a assignar
  - RESULTAT:
  - Substitueix els blobs actuals per els de l'objecte source
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: Assigment operator
  - FUNCTIONALITY:
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult& CBlobResult::operator=(const CBlobResult& source)
  {
    // si ja són el mateix, no cal fer res
    if (this != &source)
    {
      // alliberem el conjunt de blobs antic
      for (int i = 0; i < GetNumBlobs(); i++)
      {
        delete m_blobs[i];
      }
      m_blobs.clear();
      // creem el nou a partir del passat com a paràmetre
      m_blobs = blob_vector(source.GetNumBlobs());
      // copiem els blobs de l'origen a l'actual
      blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
      blob_vector::iterator pBlobsDst = m_blobs.begin();

      while (pBlobsSrc != source.m_blobs.end())
      {
        // no podem cridar a l'operador = ja que blob_vector és un 
        // vector de CBlob*. Per tant, creem un blob nou a partir del
        // blob original
        *pBlobsDst = new CBlob(**pBlobsSrc);
        ++pBlobsSrc;
        ++pBlobsDst;
      }
    }
    return *this;
  }


  /**
  - FUNCIÓ: operador +
  - FUNCIONALITAT: Concatena els blobs de dos CBlobResult
  - PARÀMETRES:
  - source: d'on s'agafaran els blobs afegits a l'actual
  - RESULTAT:
  - retorna un nou CBlobResult amb els dos CBlobResult concatenats
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - NOTA: per la implementació, els blobs del paràmetre es posen en ordre invers
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: + operator
  - FUNCTIONALITY: Joins the blobs in source with the current ones
  - PARAMETERS:
  - source: object to copy the blobs
  - RESULT:
  - object with the actual blobs and the source blobs
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlobResult CBlobResult::operator+(const CBlobResult& source)
  {
    //creem el resultat a partir dels blobs actuals
    CBlobResult resultat(*this);

    // reservem memòria per als nous blobs
    resultat.m_blobs.resize(resultat.GetNumBlobs() + source.GetNumBlobs());

    // declarem els iterador per recòrrer els blobs d'origen i desti
    blob_vector::const_iterator pBlobsSrc = source.m_blobs.begin();
    blob_vector::iterator pBlobsDst = resultat.m_blobs.end();

    // insertem els blobs de l'origen a l'actual
    while (pBlobsSrc != source.m_blobs.end())
    {
      --pBlobsDst;
      *pBlobsDst = new CBlob(**pBlobsSrc);
      ++pBlobsSrc;
    }

    return resultat;
  }

  /**************************************************************************
  Operacions / Operations
  **************************************************************************/

  /**
  - FUNCIÓ: AddBlob
  - FUNCIONALITAT: Afegeix un blob al conjunt
  - PARÀMETRES:
  - blob: blob a afegir
  - RESULTAT:
  - modifica el conjunt de blobs actual
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 2006/03/01
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  void CBlobResult::AddBlob(CBlob *blob)
  {
    if (blob != NULL)
      m_blobs.push_back(new CBlob(blob));
  }


  /**
  - FUNCIÓ: GetSTLResult
  - FUNCIONALITAT: Calcula el resultat especificat sobre tots els blobs de la classe
  - PARÀMETRES:
  - evaluador: Qualsevol objecte derivat de COperadorBlob
  - RESULTAT:
  - Retorna un array de double's STL amb el resultat per cada blob
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: GetResult
  - FUNCTIONALITY: Computes the function evaluador on all the blobs of the class
  and returns a vector with the result
  - PARAMETERS:
  - evaluador: function to apply to each blob (any object derived from the
  COperadorBlob class )
  - RESULT:
  - vector with all the results in the same order as the blobs
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double_stl_vector CBlobResult::GetSTLResult(funcio_calculBlob *evaluador) const
  {
    if (GetNumBlobs() <= 0)
    {
      return double_stl_vector();
    }

    // definim el resultat
    double_stl_vector result = double_stl_vector(GetNumBlobs());
    // i iteradors sobre els blobs i el resultat
    double_stl_vector::iterator itResult = result.begin();
    blob_vector::const_iterator itBlobs = m_blobs.begin();

    // avaluem la funció en tots els blobs
    while (itBlobs != m_blobs.end())
    {
      *itResult = (*evaluador)(**itBlobs);
      ++itBlobs;
      ++itResult;
    }
    return result;
  }

  /**
  - FUNCIÓ: GetNumber
  - FUNCIONALITAT: Calcula el resultat especificat sobre un únic blob de la classe
  - PARÀMETRES:
  - evaluador: Qualsevol objecte derivat de COperadorBlob
  - indexblob: número de blob del que volem calcular el resultat.
  - RESULTAT:
  - Retorna un double amb el resultat
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: GetNumber
  - FUNCTIONALITY: Computes the function evaluador on a blob of the class
  - PARAMETERS:
  - indexBlob: index of the blob to compute the function
  - evaluador: function to apply to each blob (any object derived from the
  COperadorBlob class )
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobResult::GetNumber(int indexBlob, funcio_calculBlob *evaluador) const
  {
    if (indexBlob < 0 || indexBlob >= GetNumBlobs())
      RaiseError(EXCEPTION_BLOB_OUT_OF_BOUNDS);
    return (*evaluador)(*m_blobs[indexBlob]);
  }

  /////////////////////////// FILTRAT DE BLOBS ////////////////////////////////////

  /**
  - FUNCIÓ: Filter
  - FUNCIONALITAT: Filtra els blobs de la classe i deixa el resultat amb només
  els blobs que han passat el filtre.
  El filtrat es basa en especificar condicions sobre un resultat dels blobs
  i seleccionar (o excloure) aquells blobs que no compleixen una determinada
  condicio
  - PARÀMETRES:
  - dst: variable per deixar els blobs filtrats
  - filterAction:	acció de filtrat. Incloure els blobs trobats (B_INCLUDE),
  o excloure els blobs trobats (B_EXCLUDE)
  - evaluador: Funció per evaluar els blobs (qualsevol objecte derivat de COperadorBlob
  - Condition: tipus de condició que ha de superar la mesura (FilterType)
  sobre cada blob per a ser considerat.
  B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
  B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
  - LowLimit:  valor numèric per a la comparació (Condition) de la mesura (FilterType)
  - HighLimit: valor numèric per a la comparació (Condition) de la mesura (FilterType)
  (només té sentit per a aquelles condicions que tenen dos valors
  (B_INSIDE, per exemple).
  - RESULTAT:
  - Deixa els blobs resultants del filtrat a destination
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: Filter
  - FUNCTIONALITY: Get some blobs from the class based on conditions on measures
  of the blobs.
  - PARAMETERS:
  - dst: where to store the selected blobs
  - filterAction:	B_INCLUDE: include the blobs which pass the filter in the result
  B_EXCLUDE: exclude the blobs which pass the filter in the result
  - evaluador: Object to evaluate the blob
  - Condition: How to decide if  the result returned by evaluador on each blob
  is included or not. It can be:
  B_EQUAL,B_NOT_EQUAL,B_GREATER,B_LESS,B_GREATER_OR_EQUAL,
  B_LESS_OR_EQUAL,B_INSIDE,B_OUTSIDE
  - LowLimit:  numerical value to evaluate the Condition on evaluador(blob)
  - HighLimit: numerical value to evaluate the Condition on evaluador(blob).
  Only useful for B_INSIDE and B_OUTSIDE
  - RESULT:
  - It returns on dst the blobs that accomplish (B_INCLUDE) or discards (B_EXCLUDE)
  the Condition on the result returned by evaluador on each blob
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlobResult::Filter(CBlobResult &dst,
    int filterAction,
    funcio_calculBlob *evaluador,
    int condition,
    double lowLimit, double highLimit /*=0*/)

  {
    int i, numBlobs;
    bool resultavaluacio;
    double_stl_vector avaluacioBlobs;
    double_stl_vector::iterator itavaluacioBlobs;

    if (GetNumBlobs() <= 0) return;
    if (!evaluador) return;
    //avaluem els blobs amb la funció pertinent	
    avaluacioBlobs = GetSTLResult(evaluador);
    itavaluacioBlobs = avaluacioBlobs.begin();
    numBlobs = GetNumBlobs();
    switch (condition)
    {
    case B_EQUAL:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs == lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_NOT_EQUAL:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs != lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_GREATER:
      for (i = 0; i<numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs > lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_LESS:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs < lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_GREATER_OR_EQUAL:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs >= lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_LESS_OR_EQUAL:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = *itavaluacioBlobs <= lowLimit;
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_INSIDE:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = (*itavaluacioBlobs >= lowLimit) && (*itavaluacioBlobs <= highLimit);
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    case B_OUTSIDE:
      for (i = 0; i < numBlobs; i++, itavaluacioBlobs++)
      {
        resultavaluacio = (*itavaluacioBlobs < lowLimit) || (*itavaluacioBlobs > highLimit);
        if ((resultavaluacio && filterAction == B_INCLUDE) ||
          (!resultavaluacio && filterAction == B_EXCLUDE))
        {
          dst.m_blobs.push_back(new CBlob(GetBlob(i)));
        }
      }
      break;
    }


    // en cas de voler filtrar un CBlobResult i deixar-ho en el mateix CBlobResult
    // ( operacio inline )
    if (&dst == this)
    {
      // esborrem els primers blobs ( que són els originals )
      // ja que els tindrem replicats al final si passen el filtre
      blob_vector::iterator itBlobs = m_blobs.begin();
      for (int i = 0; i < numBlobs; i++)
      {
        delete *itBlobs;
        ++itBlobs;
      }
      m_blobs.erase(m_blobs.begin(), itBlobs);
    }
  }


  /**
  - FUNCIÓ: GetBlob
  - FUNCIONALITAT: Retorna un blob si aquest existeix (index != -1)
  - PARÀMETRES:
  - indexblob: index del blob a retornar
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /*
  - FUNCTION: GetBlob
  - FUNCTIONALITY: Gets the n-th blob (without ordering the blobs)
  - PARAMETERS:
  - indexblob: index in the blob array
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlob CBlobResult::GetBlob(int indexblob) const
  {
    if (indexblob < 0 || indexblob >= GetNumBlobs())
      RaiseError(EXCEPTION_BLOB_OUT_OF_BOUNDS);

    return *m_blobs[indexblob];
  }
  CBlob *CBlobResult::GetBlob(int indexblob)
  {
    if (indexblob < 0 || indexblob >= GetNumBlobs())
      RaiseError(EXCEPTION_BLOB_OUT_OF_BOUNDS);

    return m_blobs[indexblob];
  }

  /**
  - FUNCIÓ: GetNthBlob
  - FUNCIONALITAT: Retorna l'enèssim blob segons un determinat criteri
  - PARÀMETRES:
  - criteri: criteri per ordenar els blobs (objectes derivats de COperadorBlob)
  - nBlob: index del blob a retornar
  - dst: on es retorna el resultat
  - RESULTAT:
  - retorna el blob nBlob a dst ordenant els blobs de la classe segons el criteri
  en ordre DESCENDENT. Per exemple, per obtenir el blob major:
  GetNthBlob( CBlobGetArea(), 0, blobMajor );
  GetNthBlob( CBlobGetArea(), 1, blobMajor ); (segon blob més gran)
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /*
  - FUNCTION: GetNthBlob
  - FUNCTIONALITY: Gets the n-th blob ordering first the blobs with some criteria
  - PARAMETERS:
  - criteri: criteria to order the blob array
  - nBlob: index of the returned blob in the ordered blob array
  - dst: where to store the result
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlobResult::GetNthBlob(funcio_calculBlob *criteri, int nBlob, CBlob &dst) const
  {
    // verifiquem que no estem accedint fora el vector de blobs
    if (nBlob < 0 || nBlob >= GetNumBlobs())
    {
      //RaiseError( EXCEPTION_BLOB_OUT_OF_BOUNDS );
      dst = CBlob();
      return;
    }

    double_stl_vector avaluacioBlobs, avaluacioBlobsOrdenat;
    double valorEnessim;

    //avaluem els blobs amb la funció pertinent	
    avaluacioBlobs = GetSTLResult(criteri);

    avaluacioBlobsOrdenat = double_stl_vector(GetNumBlobs());

    // obtenim els nBlob primers resultats (en ordre descendent)
    std::partial_sort_copy(avaluacioBlobs.begin(),
      avaluacioBlobs.end(),
      avaluacioBlobsOrdenat.begin(),
      avaluacioBlobsOrdenat.end(),
      std::greater<double>());

    valorEnessim = avaluacioBlobsOrdenat[nBlob];

    // busquem el primer blob que té el valor n-ssim
    double_stl_vector::const_iterator itAvaluacio = avaluacioBlobs.begin();

    bool trobatBlob = false;
    int indexBlob = 0;
    while (itAvaluacio != avaluacioBlobs.end() && !trobatBlob)
    {
      if (*itAvaluacio == valorEnessim)
      {
        trobatBlob = true;
        dst = CBlob(GetBlob(indexBlob));
      }
      ++itAvaluacio;
      indexBlob++;
    }
  }

  /**
  - FUNCIÓ: ClearBlobs
  - FUNCIONALITAT: Elimina tots els blobs de l'objecte
  - PARÀMETRES:
  - RESULTAT:
  - Allibera tota la memòria dels blobs
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs Navarra
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /*
  - FUNCTION: ClearBlobs
  - FUNCTIONALITY: Clears all the blobs from the object and releases all its memory
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlobResult::ClearBlobs()
  {
    /*for( int i = 0; i < GetNumBlobs(); i++ )
    {
    delete m_blobs[i];
    }*/
    blob_vector::iterator itBlobs = m_blobs.begin();
    while (itBlobs != m_blobs.end())
    {
      delete *itBlobs;
      ++itBlobs;
    }

    m_blobs.clear();
  }

  /**
  - FUNCIÓ: RaiseError
  - FUNCIONALITAT: Funció per a notificar errors al l'usuari (en debug) i llença
  les excepcions
  - PARÀMETRES:
  - errorCode: codi d'error
  - RESULTAT:
  - Ensenya un missatge a l'usuari (en debug) i llença una excepció
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs Navarra
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /*
  - FUNCTION: RaiseError
  - FUNCTIONALITY: Error handling function
  - PARAMETERS:
  - errorCode: reason of the error
  - RESULT:
  - in _DEBUG version, shows a message box with the error. In release is silent.
  In both cases throws an exception with the error.
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlobResult::RaiseError(const int errorCode) const
  {
    throw errorCode;
  }



  /**************************************************************************
  Auxiliars / Auxiliary functions
  **************************************************************************/


  /**
  - FUNCIÓ: PrintBlobs
  - FUNCIONALITAT: Escriu els paràmetres (àrea, perímetre, exterior, mitjana)
  de tots els blobs a un fitxer.
  - PARÀMETRES:
  - nom_fitxer: path complet del fitxer amb el resultat
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /*
  - FUNCTION: PrintBlobs
  - FUNCTIONALITY: Prints some blob features in an ASCII file
  - PARAMETERS:
  - nom_fitxer: full path + filename to generate
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlobResult::PrintBlobs(char *nom_fitxer) const
  {
    double_stl_vector area, /*perimetre,*/ exterior, mitjana, compacitat, longitud,
      externPerimeter, perimetreConvex, perimetre;
    int i;
    FILE *fitxer_sortida;

    area = GetSTLResult(CBlobGetArea());
    perimetre = GetSTLResult(CBlobGetPerimeter());
    exterior = GetSTLResult(CBlobGetExterior());
    mitjana = GetSTLResult(CBlobGetMean());
    compacitat = GetSTLResult(CBlobGetCompactness());
    longitud = GetSTLResult(CBlobGetLength());
    externPerimeter = GetSTLResult(CBlobGetExternPerimeter());
    perimetreConvex = GetSTLResult(CBlobGetHullPerimeter());

    fitxer_sortida = fopen(nom_fitxer, "w");

    for (i = 0; i < GetNumBlobs(); i++)
    {
      fprintf(fitxer_sortida, "blob %d ->\t a=%7.0f\t p=%8.2f (%8.2f extern)\t pconvex=%8.2f\t ext=%.0f\t m=%7.2f\t c=%3.2f\t l=%8.2f\n",
        i, area[i], perimetre[i], externPerimeter[i], perimetreConvex[i], exterior[i], mitjana[i], compacitat[i], longitud[i]);
    }
    fclose(fitxer_sortida);

  }

}
