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
Blob.cpp

- FUNCIONALITAT: Implementació de la classe CBlob
- AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):


FUNCTIONALITY: Implementation of the CBlob class and some helper classes to perform
some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/


#include <limits.h>
#include "blob.h"
#include <opencv2/opencv.hpp>

namespace Blob
{

  /**
  - FUNCIÓ: CBlob
  - FUNCIONALITAT: Constructor estàndard
  - PARÀMETRES:
  - RESULTAT:
  - inicialització de totes les variables internes i de l'storage i la sequencia
  per a les cantonades del blob
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlob
  - FUNCTIONALITY: Standard constructor
  - PARAMETERS:
  - RESULT:
  - memory allocation for the blob edges and initialization of member variables
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlob::CBlob()
  {
    etiqueta = -1;		// Flag indicates null region
    exterior = 0;
    area = 0.0f;
    perimeter = 0.0f;
    parent = -1;
    minx = LONG_MAX;
    maxx = 0;
    miny = LONG_MAX;
    maxy = 0;
    sumx = 0;
    sumy = 0;
    sumxx = 0;
    sumyy = 0;
    sumxy = 0;
    mean = 0;
    stddev = 0;
    externPerimeter = 0;

    m_storage = cvCreateMemStorage(0);
    edges = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2,
      sizeof(CvContour),
      sizeof(CvPoint), m_storage);
  }

  /**
  - FUNCIÓ: CBlob
  - FUNCIONALITAT: Constructor de còpia
  - PARÀMETRES:
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlob
  - FUNCTIONALITY: Copy constructor
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlob::CBlob(const CBlob &src)
  {
    // copiem les propietats del blob origen a l'actual
    etiqueta = src.etiqueta;
    exterior = src.exterior;
    area = src.Area();
    perimeter = src.Perimeter();
    parent = src.parent;
    minx = src.minx;
    maxx = src.maxx;
    miny = src.miny;
    maxy = src.maxy;
    sumx = src.sumx;
    sumy = src.sumy;
    sumxx = src.sumxx;
    sumyy = src.sumyy;
    sumxy = src.sumxy;
    mean = src.mean;
    stddev = src.stddev;
    externPerimeter = src.externPerimeter;

    // copiem els edges del blob origen a l'actual
    CvSeqReader reader;
    CvSeqWriter writer;
    CvPoint edgeactual;

    // creem una sequencia buida per als edges
    m_storage = cvCreateMemStorage(0);
    edges = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2,
      sizeof(CvContour),
      sizeof(CvPoint), m_storage);

    cvStartReadSeq(src.Edges(), &reader);
    cvStartAppendToSeq(edges, &writer);

    for (int i = 0; i < src.Edges()->total; i++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      CV_WRITE_SEQ_ELEM(edgeactual, writer);
    }

    cvEndWriteSeq(&writer);
  }
  CBlob::CBlob(const CBlob *src)
  {
    // copiem les propietats del blob origen a l'actual
    etiqueta = src->etiqueta;
    exterior = src->exterior;
    area = src->Area();
    perimeter = src->Perimeter();
    parent = src->parent;
    minx = src->minx;
    maxx = src->maxx;
    miny = src->miny;
    maxy = src->maxy;
    sumx = src->sumx;
    sumy = src->sumy;
    sumxx = src->sumxx;
    sumyy = src->sumyy;
    sumxy = src->sumxy;
    mean = src->mean;
    stddev = src->stddev;
    externPerimeter = src->externPerimeter;

    // copiem els edges del blob origen a l'actual
    CvSeqReader reader;
    CvSeqWriter writer;
    CvPoint edgeactual;

    // creem una sequencia buida per als edges
    m_storage = cvCreateMemStorage(0);
    edges = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2,
      sizeof(CvContour),
      sizeof(CvPoint), m_storage);

    cvStartReadSeq(src->Edges(), &reader);
    cvStartAppendToSeq(edges, &writer);

    for (int i = 0; i < src->Edges()->total; i++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      CV_WRITE_SEQ_ELEM(edgeactual, writer);
    }

    cvEndWriteSeq(&writer);
  }

  /**
  - FUNCIÓ: ~CBlob
  - FUNCIONALITAT: Destructor estàndard
  - PARÀMETRES:
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlob
  - FUNCTIONALITY: Standard destructor
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlob::~CBlob()
  {
    // Eliminar vèrtexs del blob 
    cvClearSeq(edges);
    // i la zona de memòria on són
    cvReleaseMemStorage(&m_storage);
  }

  /**
  - FUNCIÓ: operator=
  - FUNCIONALITAT: Operador d'assignació
  - PARÀMETRES:
  - src: blob a assignar a l'actual
  - RESULTAT:
  - Substitueix el blob actual per el src
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: Assigment operator
  - FUNCTIONALITY: Assigns a blob to the current
  - PARAMETERS:
  - src: blob to assign
  - RESULT:
  - the current blob is replaced by the src blob
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CBlob& CBlob::operator=(const CBlob &src)
  {
    // si ja són el mateix, no cal fer res
    if (this != &src)
    {
      // Eliminar vèrtexs del blob 
      cvClearSeq(edges);
      // i la zona de memòria on són
      cvReleaseMemStorage(&m_storage);

      // creem una sequencia buida per als edges
      m_storage = cvCreateMemStorage(0);
      edges = cvCreateSeq(CV_SEQ_KIND_GENERIC | CV_32SC2,
        sizeof(CvContour),
        sizeof(CvPoint), m_storage);

      // copiem les propietats del blob origen a l'actual
      etiqueta = src.etiqueta;
      exterior = src.exterior;
      area = src.Area();
      perimeter = src.Perimeter();
      parent = src.parent;
      minx = src.minx;
      maxx = src.maxx;
      miny = src.miny;
      maxy = src.maxy;
      sumx = src.sumx;
      sumy = src.sumy;
      sumxx = src.sumxx;
      sumyy = src.sumyy;
      sumxy = src.sumxy;
      mean = src.mean;
      stddev = src.stddev;
      externPerimeter = src.externPerimeter;

      // copiem els edges del blob origen a l'actual
      CvSeqReader reader;
      CvSeqWriter writer;
      CvPoint edgeactual;

      cvStartReadSeq(src.Edges(), &reader);
      cvStartAppendToSeq(edges, &writer);

      for (int i = 0; i < src.Edges()->total; i++)
      {
        CV_READ_SEQ_ELEM(edgeactual, reader);
        CV_WRITE_SEQ_ELEM(edgeactual, writer);
      }

      cvEndWriteSeq(&writer);
    }
    return *this;
  }

  /**
  - FUNCIÓ: FillBlob
  - FUNCIONALITAT: Pinta l'interior d'un blob amb el color especificat
  - PARÀMETRES:
  - imatge: imatge on es vol pintar el el blob
  - color: color amb que es vol pintar el blob
  - RESULTAT:
  - retorna la imatge d'entrada amb el blob pintat
  - RESTRICCIONS:
  - AUTOR:
  - Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: FillBlob
  - FUNCTIONALITY:
  - Fills the blob with a specified colour
  - PARAMETERS:
  - imatge: where to paint
  - color: colour to paint the blob
  - RESULT:
  - modifies input image and returns the seed point used to fill the blob
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlob::FillBlob(IplImage *imatge, CvScalar color, int offsetX /*=0*/, int offsetY /*=0*/) const
  {

    //verifiquem que existeixi el blob i que tingui cantonades
    if (edges == NULL || edges->total == 0) return;

    CvPoint edgeactual, pt1, pt2;
    CvSeqReader reader;
    vectorPunts vectorEdges = vectorPunts(edges->total);
    vectorPunts::iterator itEdges, itEdgesSeguent;
    bool dinsBlob;
    int yActual;

    // passem els punts del blob a un vector de punts de les STL
    cvStartReadSeq(edges, &reader);
    itEdges = vectorEdges.begin();
    while (itEdges != vectorEdges.end())
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      *itEdges = edgeactual;
      ++itEdges;
    }
    // ordenem el vector per les Y's i les X's d'esquerra a dreta
    std::sort(vectorEdges.begin(), vectorEdges.end(), comparaCvPoint());

    // recorrem el vector ordenat i fem linies entre punts consecutius
    itEdges = vectorEdges.begin();
    itEdgesSeguent = vectorEdges.begin() + 1;
    dinsBlob = true;
    while (itEdges != (vectorEdges.end() - 1))
    {
      yActual = (*itEdges).y;

      if (((*itEdges).x != (*itEdgesSeguent).x) &&
        ((*itEdgesSeguent).y == yActual)
        )
      {
        if (dinsBlob)
        {
          pt1 = *itEdges;
          pt1.x += offsetX;
          pt1.y += offsetY;

          pt2 = *itEdgesSeguent;
          pt2.x += offsetX;
          pt2.y += offsetY;

          cvLine(imatge, pt1, pt2, color);
        }
        dinsBlob = !dinsBlob;
      }
      ++itEdges;
      ++itEdgesSeguent;
      if ((*itEdges).y != yActual) dinsBlob = true;
    }
    vectorEdges.clear();
  }

  /**
  - FUNCIÓ: CopyEdges
  - FUNCIONALITAT: Afegeix els vèrtexs del blob al blob destination
  - PARÀMETRES:
  - destination: blob al que volem afegir els vèrtexs
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CopyEdges
  - FUNCTIONALITY: Adds the blob edges to destination
  - PARAMETERS:
  - destination: where to add the edges
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlob::CopyEdges(CBlob &destination) const
  {
    CvSeqReader reader;
    CvSeqWriter writer;
    CvPoint edgeactual;

    cvStartReadSeq(edges, &reader);
    cvStartAppendToSeq(destination.Edges(), &writer);

    for (int i = 0; i < edges->total; i++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      CV_WRITE_SEQ_ELEM(edgeactual, writer);
    }

    cvEndWriteSeq(&writer);
  }

  /**
  - FUNCIÓ: ClearEdges
  - FUNCIONALITAT: Elimina els vèrtexs del blob
  - PARÀMETRES:
  - RESULTAT:
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: ClearEdges
  - FUNCTIONALITY: Delete current blob edges
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  void CBlob::ClearEdges()
  {
    // Eliminar vèrtexs del blob eliminat
    cvClearSeq(edges);
  }

  /**
  - FUNCIÓ: GetConvexHull
  - FUNCIONALITAT: Retorna el poligon convex del blob
  - PARÀMETRES:
  - dst: sequencia on desarem el resultat (no ha d'estar inicialitzada)
  - RESULTAT:
  - true si tot ha anat bé
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: GetConvexHull
  - FUNCTIONALITY: Calculates the convex hull polygon of the blob
  - PARAMETERS:
  - dst: where to store the result
  - RESULT:
  - true if no error ocurred
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  bool CBlob::GetConvexHull(CvSeq **dst) const
  {
    if (edges != NULL && edges->total > 0)
    {
      *dst = cvConvexHull2(edges, 0, CV_CLOCKWISE, 0);
      return true;
    }
    return false;
  }

  /**
  - FUNCIÓ: GetEllipse
  - FUNCIONALITAT: Retorna l'ellipse que s'ajusta millor a les cantonades del blob
  - PARÀMETRES:
  - RESULTAT:
  - estructura amb l'ellipse
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: GetEllipse
  - FUNCTIONALITY: Calculates the ellipse that best fits the edges of the blob
  - PARAMETERS:
  - RESULT:
  - CvBox2D struct with the calculated ellipse
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  CvBox2D CBlob::GetEllipse() const
  {
    CvBox2D elipse;
    // necessitem 6 punts per calcular l'elipse
    if (edges != NULL && edges->total > 6)
    {
      elipse = cvFitEllipse2(edges);
    }
    else
    {
      elipse.center.x = 0.0;
      elipse.center.y = 0.0;
      elipse.size.width = 0.0;
      elipse.size.height = 0.0;
      elipse.angle = 0.0;
    }
    return elipse;
  }



  /***************************************************************************
  Implementació de les classes per al càlcul de característiques sobre el blob

  Implementation of the helper classes to perform operations on blobs
  **************************************************************************/

  /**
  - FUNCIÓ: Moment
  - FUNCIONALITAT: Calcula el moment pq del blob
  - RESULTAT:
  - retorna el moment pq especificat o 0 si el moment no està implementat
  - RESTRICCIONS:
  - Implementats els moments pq: 00, 01, 10, 20, 02
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: Moment
  - FUNCTIONALITY: Calculates the pq moment of the blob
  - PARAMETERS:
  - RESULT:
  - returns the pq moment or 0 if the moment it is not implemented
  - RESTRICTIONS:
  - Currently, only implemented the 00, 01, 10, 20, 02 pq moments
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 20-07-2004.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetMoment::operator()(const CBlob &blob) const
  {
    //Moment 00
    if ((m_p == 0) && (m_q == 0))
      return blob.Area();

    //Moment 10
    if ((m_p == 1) && (m_q == 0))
      return blob.SumX();

    //Moment 01
    if ((m_p == 0) && (m_q == 1))
      return blob.SumY();

    //Moment 20
    if ((m_p == 2) && (m_q == 0))
      return blob.SumXX();

    //Moment 02
    if ((m_p == 0) && (m_q == 2))
      return blob.SumYY();

    return 0;
  }

  /**
  - FUNCIÓ: HullPerimeter
  - FUNCIONALITAT: Calcula la longitud del perimetre convex del blob.
  Fa servir la funció d'OpenCV cvConvexHull2 per a
  calcular el perimetre convex.

  - PARÀMETRES:
  - RESULTAT:
  - retorna la longitud del perímetre convex del blob. Si el blob no té coordenades
  associades retorna el perímetre normal del blob.
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobGetHullPerimeter
  - FUNCTIONALITY: Calculates the convex hull perimeter of the blob
  - PARAMETERS:
  - RESULT:
  - returns the convex hull perimeter of the blob or the perimeter if the
  blob edges could not be retrieved
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetHullPerimeter::operator()(const CBlob &blob) const
  {
    if (blob.Edges() != NULL && blob.Edges()->total > 0)
    {
      CvSeq *hull = cvConvexHull2(blob.Edges(), 0, CV_CLOCKWISE, 1);
      return fabs(cvArcLength(hull, CV_WHOLE_SEQ, 1));
    }
    return blob.Perimeter();
  }

  double CBlobGetHullArea::operator()(const CBlob &blob) const
  {
    if (blob.Edges() != NULL && blob.Edges()->total > 0)
    {
      CvSeq *hull = cvConvexHull2(blob.Edges(), 0, CV_CLOCKWISE, 1);
      return fabs(cvContourArea(hull));
    }
    return blob.Perimeter();
  }

  /**
  - FUNCIÓ: MinX_at_MinY
  - FUNCIONALITAT: Calcula el valor MinX a MinY.
  - PARÀMETRES:
  - blob: blob del que volem calcular el valor
  - RESULTAT:
  - retorna la X minima en la Y minima.
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobGetMinXatMinY
  - FUNCTIONALITY: Calculates the minimum X on the minimum Y
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetMinXatMinY::operator()(const CBlob &blob) const
  {
    double MinX_at_MinY = LONG_MAX;

    CvSeqReader reader;
    CvPoint edgeactual;

    cvStartReadSeq(blob.Edges(), &reader);

    for (int j = 0; j < blob.Edges()->total; j++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      if ((edgeactual.y == blob.MinY()) && (edgeactual.x < MinX_at_MinY))
      {
        MinX_at_MinY = edgeactual.x;
      }
    }

    return MinX_at_MinY;
  }

  /**
  - FUNCIÓ: MinY_at_MaxX
  - FUNCIONALITAT: Calcula el valor MinX a MaxX.
  - PARÀMETRES:
  - blob: blob del que volem calcular el valor
  - RESULTAT:
  - retorna la Y minima en la X maxima.
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobGetMinXatMinY
  - FUNCTIONALITY: Calculates the minimum Y on the maximum X
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetMinYatMaxX::operator()(const CBlob &blob) const
  {
    double MinY_at_MaxX = LONG_MAX;

    CvSeqReader reader;
    CvPoint edgeactual;

    cvStartReadSeq(blob.Edges(), &reader);

    for (int j = 0; j < blob.Edges()->total; j++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      if ((edgeactual.x == blob.MaxX()) && (edgeactual.y < MinY_at_MaxX))
      {
        MinY_at_MaxX = edgeactual.y;
      }
    }

    return MinY_at_MaxX;
  }

  /**
  - FUNCIÓ: MaxX_at_MaxY
  - FUNCIONALITAT: Calcula el valor MaxX a MaxY.
  - PARÀMETRES:
  - blob: blob del que volem calcular el valor
  - RESULTAT:
  - retorna la X maxima en la Y maxima.
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobGetMaxXatMaxY
  - FUNCTIONALITY: Calculates the maximum X on the maximum Y
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetMaxXatMaxY::operator()(const CBlob &blob) const
  {
    double MaxX_at_MaxY = LONG_MIN;

    CvSeqReader reader;
    CvPoint edgeactual;

    cvStartReadSeq(blob.Edges(), &reader);

    for (int j = 0; j<blob.Edges()->total; j++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      if ((edgeactual.y == blob.MaxY()) && (edgeactual.x > MaxX_at_MaxY))
      {
        MaxX_at_MaxY = edgeactual.x;
      }
    }

    return MaxX_at_MaxY;
  }

  /**
  - FUNCIÓ: MaxY_at_MinX
  - FUNCIONALITAT: Calcula el valor MaxY a MinX.
  - PARÀMETRES:
  - blob: blob del que volem calcular el valor
  - RESULTAT:
  - retorna la Y maxima en la X minima.
  - RESTRICCIONS:
  - AUTOR: Ricard Borràs
  - DATA DE CREACIÓ: 20-07-2004.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: CBlobGetMaxYatMinX
  - FUNCTIONALITY: Calculates the maximum Y on the minimum X
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetMaxYatMinX::operator()(const CBlob &blob) const
  {
    double MaxY_at_MinX = LONG_MIN;

    CvSeqReader reader;
    CvPoint edgeactual;

    cvStartReadSeq(blob.Edges(), &reader);

    for (int j = 0; j<blob.Edges()->total; j++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      if ((edgeactual.x == blob.MinY()) && (edgeactual.y > MaxY_at_MinX))
      {
        MaxY_at_MinX = edgeactual.y;
      }
    }

    return MaxY_at_MinX;
  }

  /**
  Retorna l'elongació del blob (longitud/amplada)
  */
  /**
  - FUNCTION: CBlobGetElongation
  - FUNCTIONALITY: Calculates the elongation of the blob ( length/breadth )
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - See below to see how the lenght and the breadth are aproximated
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetElongation::operator()(const CBlob &blob) const
  {
    double ampladaC, longitudC, amplada, longitud;

    ampladaC = (double)(blob.Perimeter() + sqrt(pow(blob.Perimeter(), 2) - 16 * blob.Area())) / 4;
    if (ampladaC <= 0.0) return 0;
    longitudC = (double)blob.Area() / ampladaC;

    longitud = MAX(longitudC, ampladaC);
    amplada = MIN(longitudC, ampladaC);

    return (double)longitud / amplada;
  }

  /**
  Retorna la compacitat del blob
  */
  /**
  - FUNCTION: CBlobGetCompactness
  - FUNCTIONALITY: Calculates the compactness of the blob
  ( maximum for circle shaped blobs, minimum for the rest)
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetCompactness::operator()(const CBlob &blob) const
  {
    if (blob.Area() != 0.0)
      return (double)pow(blob.Perimeter(), 2) / (4 * CV_PI*blob.Area());
    else
      return 0.0;
  }

  /**
  Retorna la rugositat del blob
  */
  /**
  - FUNCTION: CBlobGetRoughness
  - FUNCTIONALITY: Calculates the roughness of the blob
  ( ratio between perimeter and convex hull perimeter)
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetRoughness::operator()(const CBlob &blob) const
  {
    CBlobGetHullPerimeter getHullPerimeter = CBlobGetHullPerimeter();

    double hullPerimeter = getHullPerimeter(blob);

    if (hullPerimeter != 0.0)
      return blob.Perimeter() / hullPerimeter;//HullPerimeter();

    return 0.0;
  }

  /**
  Retorna la longitud del blob
  */
  /**
  - FUNCTION: CBlobGetLength
  - FUNCTIONALITY: Calculates the lenght of the blob (the biggest axis of the blob)
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - The lenght is an aproximation to the real lenght
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetLength::operator()(const CBlob &blob) const
  {
    double ampladaC, longitudC;
    double tmp;

    tmp = blob.Perimeter()*blob.Perimeter() - 16 * blob.Area();

    if (tmp > 0.0)
      ampladaC = (double)(blob.Perimeter() + sqrt(tmp)) / 4;
    // error intrínsec en els càlculs de l'àrea i el perímetre 
    else
      ampladaC = (double)(blob.Perimeter()) / 4;

    if (ampladaC <= 0.0) return 0;
    longitudC = (double)blob.Area() / ampladaC;

    return MAX(longitudC, ampladaC);
  }

  /**
  Retorna l'amplada del blob
  */
  /**
  - FUNCTION: CBlobGetBreadth
  - FUNCTIONALITY: Calculates the breadth of the blob (the smallest axis of the blob)
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - The breadth is an aproximation to the real breadth
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetBreadth::operator()(const CBlob &blob) const
  {
    double ampladaC, longitudC;
    double tmp;

    tmp = blob.Perimeter()*blob.Perimeter() - 16 * blob.Area();

    if (tmp > 0.0)
      ampladaC = (double)(blob.Perimeter() + sqrt(tmp)) / 4;
    // error intrínsec en els càlculs de l'àrea i el perímetre 
    else
      ampladaC = (double)(blob.Perimeter()) / 4;

    if (ampladaC <= 0.0) return 0;
    longitudC = (double)blob.Area() / ampladaC;

    return MIN(longitudC, ampladaC);
  }

  /**
  Calcula la distància entre un punt i el centre del blob
  */
  /**
  - FUNCTION: CBlobGetDistanceFromPoint
  - FUNCTIONALITY: Calculates the euclidean distance between the blob center and
  the point specified in the constructor
  - PARAMETERS:
  - RESULT:
  - RESTRICTIONS:
  - AUTHOR: Ricard Borràs
  - CREATION DATE: 25-05-2005.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetDistanceFromPoint::operator()(const CBlob &blob) const
  {
    double xmitjana, ymitjana;
    CBlobGetXCenter getXCenter;
    CBlobGetYCenter getYCenter;

    xmitjana = m_x - getXCenter(blob);
    ymitjana = m_y - getYCenter(blob);

    return sqrt((xmitjana*xmitjana) + (ymitjana*ymitjana));
  }

  /**
  - FUNCIÓ: BlobGetXYInside
  - FUNCIONALITAT: Calcula si un punt cau dins de la capsa rectangular
  del blob
  - RESULTAT:
  - retorna 1 si hi està; 0 si no
  - RESTRICCIONS:
  - AUTOR: Francesc Pinyol Margalef
  - DATA DE CREACIÓ: 16-01-2006.
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  /**
  - FUNCTION: BlobGetXYInside
  - FUNCTIONALITY: Calculates whether a point is inside the
  rectangular bounding box of a blob
  - PARAMETERS:
  - RESULT:
  - returns 1 if it is inside; o if not
  - RESTRICTIONS:
  - AUTHOR: Francesc Pinyol Margalef
  - CREATION DATE: 16-01-2006.
  - MODIFICATION: Date. Author. Description.
  */
  double CBlobGetXYInside::operator()(const CBlob &blob) const
  {
    if (blob.Edges() == NULL || blob.Edges()->total == 0) return 0.0;

    // passem els punts del blob a un vector de punts de les STL
    CvSeqReader reader;
    CBlob::vectorPunts vectorEdges;
    CBlob::vectorPunts::iterator itEdges, itEdgesSeguent;
    CvPoint edgeactual;
    bool dinsBlob;

    // agafem tots els punts amb la mateixa y que l'actual
    cvStartReadSeq(blob.Edges(), &reader);

    for (int i = 0; i < blob.Edges()->total; i++)
    {
      CV_READ_SEQ_ELEM(edgeactual, reader);
      if (edgeactual.y == m_p.y)
        vectorEdges.push_back(edgeactual);
    }

    if (vectorEdges.empty()) return 0.0;

    // ordenem el vector per les Y's i les X's d'esquerra a dreta
    std::sort(vectorEdges.begin(), vectorEdges.end(), CBlob::comparaCvPoint());

    // recorrem el punts del blob de la mateixa fila que el punt d'entrada
    // i mirem si la X del punt d'entrada està entre dos coordenades "plenes"
    // del blob
    itEdges = vectorEdges.begin();
    itEdgesSeguent = vectorEdges.begin() + 1;
    dinsBlob = true;

    while (itEdges != (vectorEdges.end() - 1))
    {
      if ((*itEdges).x <= m_p.x && (*itEdgesSeguent).x >= m_p.x && dinsBlob)
      {
        vectorEdges.clear();
        return 1.0;
      }

      ++itEdges;
      ++itEdgesSeguent;
      dinsBlob = !dinsBlob;
    }

    vectorEdges.clear();
    return 0.0;
  }

#ifdef BLOB_OBJECT_FACTORY

  /**
  - FUNCIÓ: RegistraTotsOperadors
  - FUNCIONALITAT: Registrar tots els operadors definits a blob.h
  - PARÀMETRES:
  - fabricaOperadorsBlob: fàbrica on es registraran els operadors
  - RESULTAT:
  - Modifica l'objecte fabricaOperadorsBlob
  - RESTRICCIONS:
  - Només es registraran els operadors de blob.h. Si se'n volen afegir, cal afegir-los amb 
  el mètode Register de la fàbrica.
  - AUTOR: rborras
  - DATA DE CREACIÓ: 2006/05/18
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob )
  {
    // blob shape
    fabricaOperadorsBlob.Register( CBlobGetArea().GetNom(), Type2Type<CBlobGetArea>());
    fabricaOperadorsBlob.Register( CBlobGetBreadth().GetNom(), Type2Type<CBlobGetBreadth>());
    fabricaOperadorsBlob.Register( CBlobGetCompactness().GetNom(), Type2Type<CBlobGetCompactness>());
    fabricaOperadorsBlob.Register( CBlobGetElongation().GetNom(), Type2Type<CBlobGetElongation>());
    fabricaOperadorsBlob.Register( CBlobGetExterior().GetNom(), Type2Type<CBlobGetExterior>());
    fabricaOperadorsBlob.Register( CBlobGetLength().GetNom(), Type2Type<CBlobGetLength>());
    fabricaOperadorsBlob.Register( CBlobGetPerimeter().GetNom(), Type2Type<CBlobGetPerimeter>());
    fabricaOperadorsBlob.Register( CBlobGetRoughness().GetNom(), Type2Type<CBlobGetRoughness>());

    // extern pixels
    fabricaOperadorsBlob.Register( CBlobGetExternPerimeterRatio().GetNom(), Type2Type<CBlobGetExternPerimeterRatio>());
    fabricaOperadorsBlob.Register( CBlobGetExternHullPerimeterRatio().GetNom(), Type2Type<CBlobGetExternHullPerimeterRatio>());
    fabricaOperadorsBlob.Register( CBlobGetExternPerimeter().GetNom(), Type2Type<CBlobGetExternPerimeter>());


    // hull 
    fabricaOperadorsBlob.Register( CBlobGetHullPerimeter().GetNom(), Type2Type<CBlobGetHullPerimeter>());
    fabricaOperadorsBlob.Register( CBlobGetHullArea().GetNom(), Type2Type<CBlobGetHullArea>());


    // elipse info
    fabricaOperadorsBlob.Register( CBlobGetMajorAxisLength().GetNom(), Type2Type<CBlobGetMajorAxisLength>());
    fabricaOperadorsBlob.Register( CBlobGetMinorAxisLength().GetNom(), Type2Type<CBlobGetMinorAxisLength>());
    fabricaOperadorsBlob.Register( CBlobGetAxisRatio().GetNom(), Type2Type<CBlobGetAxisRatio>());
    fabricaOperadorsBlob.Register( CBlobGetOrientation().GetNom(), Type2Type<CBlobGetOrientation>());
    fabricaOperadorsBlob.Register( CBlobGetOrientationCos().GetNom(), Type2Type<CBlobGetOrientationCos>());
    fabricaOperadorsBlob.Register( CBlobGetAreaElipseRatio().GetNom(), Type2Type<CBlobGetAreaElipseRatio>());

    // min an max
    fabricaOperadorsBlob.Register( CBlobGetMaxX().GetNom(), Type2Type<CBlobGetMaxX>());
    fabricaOperadorsBlob.Register( CBlobGetMaxY().GetNom(), Type2Type<CBlobGetMaxY>());
    fabricaOperadorsBlob.Register( CBlobGetMinX().GetNom(), Type2Type<CBlobGetMinX>());
    fabricaOperadorsBlob.Register( CBlobGetMinY().GetNom(), Type2Type<CBlobGetMinY>());

    fabricaOperadorsBlob.Register( CBlobGetMaxXatMaxY().GetNom(), Type2Type<CBlobGetMaxXatMaxY>());
    fabricaOperadorsBlob.Register( CBlobGetMaxYatMinX().GetNom(), Type2Type<CBlobGetMaxYatMinX>());
    fabricaOperadorsBlob.Register( CBlobGetMinXatMinY().GetNom(), Type2Type<CBlobGetMinXatMinY>());
    fabricaOperadorsBlob.Register( CBlobGetMinYatMaxX().GetNom(), Type2Type<CBlobGetMinYatMaxX>());

    // grey level stats
    fabricaOperadorsBlob.Register( CBlobGetMean().GetNom(), Type2Type<CBlobGetMean>());
    fabricaOperadorsBlob.Register( CBlobGetStdDev().GetNom(), Type2Type<CBlobGetStdDev>());

    // coordinate info
    fabricaOperadorsBlob.Register( CBlobGetXYInside().GetNom(), Type2Type<CBlobGetXYInside>());
    fabricaOperadorsBlob.Register( CBlobGetDiffY().GetNom(), Type2Type<CBlobGetDiffY>());
    fabricaOperadorsBlob.Register( CBlobGetDiffX().GetNom(), Type2Type<CBlobGetDiffX>());
    fabricaOperadorsBlob.Register( CBlobGetXCenter().GetNom(), Type2Type<CBlobGetXCenter>());
    fabricaOperadorsBlob.Register( CBlobGetYCenter().GetNom(), Type2Type<CBlobGetYCenter>());
    fabricaOperadorsBlob.Register( CBlobGetDistanceFromPoint().GetNom(), Type2Type<CBlobGetDistanceFromPoint>());

    // moments
    fabricaOperadorsBlob.Register( CBlobGetMoment().GetNom(), Type2Type<CBlobGetMoment>());

  }

#endif

}

