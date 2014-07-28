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
Blob.h

FUNCIONALITAT: Definició de la classe CBlob
AUTOR: Inspecta S.L.
MODIFICACIONS (Modificació, Autor, Data):

FUNCTIONALITY: Definition of the CBlob class and some helper classes to perform
some calculations on it
AUTHOR: Inspecta S.L.
MODIFICATIONS (Modification, Author, Date):

**************************************************************************/

//! Disable warnings referred to 255 character truncation for the std:map
//#pragma warning( disable : 4786 ) 

#ifndef CBLOB_INSPECTA_INCLUDED
#define CBLOB_INSPECTA_INCLUDED

//#include "cxcore.h"
#include "BlobLibraryConfiguration.h"
#include <functional>
#include <vector>
#include <algorithm>
#include <opencv2/core/types_c.h>
//! Factor de conversió de graus a radians
#define DEGREE2RAD		(CV_PI / 180.0)

namespace Blob
{

  /**
  Classe que representa un blob, entés com un conjunt de pixels del
  mateix color contigus en una imatge binaritzada.

  Class to represent a blob, a group of connected pixels in a binary image
  */
  class CBlob
  {
  public:
    //! Constructor estàndard
    //! Standard constructor
    CBlob();
    //! Constructor de còpia
    //! Copy constructor
    CBlob(const CBlob &src);
    CBlob(const CBlob *src);

    //! Destructor estàndard
    //! Standard Destructor
    ~CBlob();

    //! Operador d'assignació
    //! Assigment operator
    CBlob& operator=(const CBlob &src);

    //! Indica si el blob està buit ( no té cap info associada )
    //! Shows if the blob has associated information
    bool IsEmpty() const
    {
      return (area == 0.0 && perimeter == 0.0);
    };

    //! Neteja les cantonades del blob
    //! Clears the edges of the blob
    void ClearEdges();
    //! Copia les cantonades del blob a un altre (les afegeix al destí)
    //! Adds the blob edges to another blob
    void CopyEdges(CBlob &destination) const;
    //! Retorna el poligon convex del blob
    //! Calculates the convex hull of the blob
    bool GetConvexHull(CvSeq **dst) const;
    //! Calcula l'elipse que s'adapta als vèrtexs del blob
    //! Fits an ellipse to the blob edges
    CvBox2D GetEllipse() const;

    //! Pinta l'interior d'un blob d'un color determinat
    //! Paints the blob in an image
    void FillBlob(IplImage *imatge, CvScalar color, int offsetX = 0, int offsetY = 0) const;

    //! Funcions GET sobre els valors dels blobs
    //! Get functions

    inline int Label() const	{ return etiqueta; }
    inline int Parent() const	{ return parent; }
    inline double Area() const { return area; }
    inline double Perimeter() const { return perimeter; }
    inline double ExternPerimeter() const { return externPerimeter; }
    inline int	  Exterior() const { return exterior; }
    inline double Mean() const { return mean; }
    inline double StdDev() const { return stddev; }
    inline double MinX() const { return minx; }
    inline double MinY() const { return miny; }
    inline double MaxX() const { return maxx; }
    inline double MaxY() const { return maxy; }
    inline CvSeq *Edges() const { return edges; }
    inline double SumX() const { return sumx; }
    inline double SumY() const { return sumy; }
    inline double SumXX() const { return sumxx; }
    inline double SumYY() const { return sumyy; }
    inline double SumXY() const { return sumxy; }

    //! etiqueta del blob
    //! label of the blob
    int etiqueta;
    //! flag per indicar si es exterior o no
    //! true for extern blobs
    int exterior;
    //! area del blob
    //! Blob area
    double area;
    //! perimetre del blob
    //! Blob perimeter
    double perimeter;
    //! quantitat de perimetre del blob extern
    //! amount of blob perimeter which is exterior
    double externPerimeter;
    //! etiqueta del blob pare
    //! label of the parent blob
    int parent;
    //! moments
    double sumx;
    double sumy;
    double sumxx;
    double sumyy;
    double sumxy;
    //! Bounding rect
    double minx;
    double maxx;
    double miny;
    double maxy;

    //! mitjana
    //! mean of the grey scale values of the blob pixels
    double mean;
    //! desviació standard
    //! standard deviation of the grey scale values of the blob pixels
    double stddev;

    //! àrea de memòria on es desaran els punts de contorn del blob
    //! storage which contains the edges of the blob
    CvMemStorage *m_storage;
    //!	Sequència de punts del contorn del blob
    //! Sequence with the edges of the blob
    CvSeq *edges;


    //! Point datatype for plotting (FillBlob)
    typedef std::vector<CvPoint> vectorPunts;

    //! Helper class to compare two CvPoints (for sorting in FillBlob)
    struct comparaCvPoint : public std::binary_function<CvPoint, CvPoint, bool>
    {
      //! Definim que un punt és menor com més amunt a la dreta estigui
      bool operator()(CvPoint a, CvPoint b)
      {
        if (a.y == b.y)
          return a.x < b.x;
        else
          return a.y < b.y;
      }
    };
  };



  /**************************************************************************
  Definició de les classes per a fer operacions sobre els blobs

  Helper classes to perform operations on blobs
  **************************************************************************/


  //! Classe d'on derivarem totes les operacions sobre els blobs
  //! Interface to derive all blob operations
  class COperadorBlob
  {
  public:
    virtual ~COperadorBlob(){};

    //! Aplica l'operació al blob
    virtual double operator()(const CBlob &blob) const = 0;
    //! Obté el nom de l'operador
    virtual const char *GetNom() const = 0;

    operator COperadorBlob*() const
    {
      return (COperadorBlob*)this;
    }
  };

  typedef COperadorBlob funcio_calculBlob;

#ifdef BLOB_OBJECT_FACTORY
  /**
  Funció per comparar dos identificadors dins de la fàbrica de COperadorBlobs
  */
  struct functorComparacioIdOperador
  {
    bool operator()(const char* s1, const char* s2) const
    {
      return strcmp(s1, s2) < 0;
    }
  };

  //! Definition of Object factory type for COperadorBlob objects
  typedef ObjectFactory<COperadorBlob, const char *, functorComparacioIdOperador > t_OperadorBlobFactory;

  //! Funció global per a registrar tots els operadors definits a blob.h
  void RegistraTotsOperadors( t_OperadorBlobFactory &fabricaOperadorsBlob );

#endif

  //! Classe per calcular l'àrea d'un blob
  //! Class to get the area of a blob
  class CBlobGetArea : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.Area();
    }
    const char *GetNom() const
    {
      return "CBlobGetArea";
    }
  };

  //! Classe per calcular el perimetre d'un blob
  //! Class to get the perimeter of a blob
  class CBlobGetPerimeter : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.Perimeter();
    }
    const char *GetNom() const
    {
      return "CBlobGetPerimeter";
    }
  };

  //! Classe que diu si un blob és extern o no
  //! Class to get the extern flag of a blob
  class CBlobGetExterior : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.Exterior();
    }
    const char *GetNom() const
    {
      return "CBlobGetExterior";
    }
  };

  //! Classe per calcular la mitjana de nivells de gris d'un blob
  //! Class to get the mean grey level of a blob
  class CBlobGetMean : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.Mean();
    }
    const char *GetNom() const
    {
      return "CBlobGetMean";
    }
  };

  //! Classe per calcular la desviació estàndard dels nivells de gris d'un blob
  //! Class to get the standard deviation of the grey level values of a blob
  class CBlobGetStdDev : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.StdDev();
    }
    const char *GetNom() const
    {
      return "CBlobGetStdDev";
    }
  };

  //! Classe per calcular la compacitat d'un blob
  //! Class to calculate the compactness of a blob
  class CBlobGetCompactness : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetCompactness";
    }
  };

  //! Classe per calcular la longitud d'un blob
  //! Class to calculate the length of a blob
  class CBlobGetLength : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetLength";
    }
  };

  //! Classe per calcular l'amplada d'un blob
  //! Class to calculate the breadth of a blob
  class CBlobGetBreadth : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetBreadth";
    }
  };

  //! Classe per calcular la diferència en X del blob
  class CBlobGetDiffX : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.maxx - blob.minx;
    }
    const char *GetNom() const
    {
      return "CBlobGetDiffX";
    }
  };

  //! Classe per calcular la diferència en X del blob
  class CBlobGetDiffY : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.maxy - blob.miny;
    }
    const char *GetNom() const
    {
      return "CBlobGetDiffY";
    }
  };

  //! Classe per calcular el moment PQ del blob
  //! Class to calculate the P,Q moment of a blob
  class CBlobGetMoment : public COperadorBlob
  {
  public:
    //! Constructor estàndard
    //! Standard constructor (gets the 00 moment)
    CBlobGetMoment()
    {
      m_p = m_q = 0;
    }
    //! Constructor: indiquem el moment p,q a calcular
    //! Constructor: gets the PQ moment
    CBlobGetMoment(int p, int q)
    {
      m_p = p;
      m_q = q;
    };
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetMoment";
    }

  private:
    //! moment que volem calcular
    int m_p, m_q;
  };

  //! Classe per calcular el perimetre del poligon convex d'un blob
  //! Class to calculate the convex hull perimeter of a blob
  class CBlobGetHullPerimeter : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetHullPerimeter";
    }
  };

  //! Classe per calcular l'àrea del poligon convex d'un blob
  //! Class to calculate the convex hull area of a blob
  class CBlobGetHullArea : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetHullArea";
    }
  };

  //! Classe per calcular la x minima en la y minima
  //! Class to calculate the minimum x on the minimum y
  class CBlobGetMinXatMinY : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetMinXatMinY";
    }
  };

  //! Classe per calcular la y minima en la x maxima
  //! Class to calculate the minimum y on the maximum x
  class CBlobGetMinYatMaxX : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetMinYatMaxX";
    }
  };

  //! Classe per calcular la x maxima en la y maxima
  //! Class to calculate the maximum x on the maximum y
  class CBlobGetMaxXatMaxY : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetMaxXatMaxY";
    }
  };

  //! Classe per calcular la y maxima en la x minima
  //! Class to calculate the maximum y on the minimum y
  class CBlobGetMaxYatMinX : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetMaxYatMinX";
    }
  };

  //! Classe per a calcular la x mínima
  //! Class to get the minimum x
  class CBlobGetMinX : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MinX();
    }
    const char *GetNom() const
    {
      return "CBlobGetMinX";
    }
  };

  //! Classe per a calcular la x màxima
  //! Class to get the maximum x
  class CBlobGetMaxX : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MaxX();
    }
    const char *GetNom() const
    {
      return "CBlobGetMaxX";
    }
  };

  //! Classe per a calcular la y mínima
  //! Class to get the minimum y
  class CBlobGetMinY : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MinY();
    }
    const char *GetNom() const
    {
      return "CBlobGetMinY";
    }
  };

  //! Classe per a calcular la y màxima
  //! Class to get the maximum y
  class CBlobGetMaxY : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MaxY();
    }
    const char *GetNom() const
    {
      return "CBlobGetMax";
    }
  };


  //! Classe per calcular l'elongacio d'un blob
  //! Class to calculate the elongation of the blob
  class CBlobGetElongation : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetElongation";
    }
  };

  //! Classe per calcular la rugositat d'un blob
  //! Class to calculate the roughness of the blob
  class CBlobGetRoughness : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetRoughness";
    }
  };

  //! Classe per calcular la distància entre el centre del blob i un punt donat
  //! Class to calculate the euclidean distance between the center of a blob and a given point
  class CBlobGetDistanceFromPoint : public COperadorBlob
  {
  public:
    //! Standard constructor (distance to point 0,0)
    CBlobGetDistanceFromPoint()
    {
      m_x = m_y = 0.0;
    }
    //! Constructor (distance to point x,y)
    CBlobGetDistanceFromPoint(const double x, const double y)
    {
      m_x = x;
      m_y = y;
    }

    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetDistanceFromPoint";
    }

  private:
    // coordenades del punt on volem calcular la distància
    double m_x, m_y;
  };

  //! Classe per calcular el nombre de pixels externs d'un blob
  //! Class to get the number of extern pixels of a blob
  class CBlobGetExternPerimeter : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.ExternPerimeter();
    }
    const char *GetNom() const
    {
      return "CBlobGetExternPerimeter";
    }
  };

  //! Classe per calcular el ratio entre el perimetre i nombre pixels externs
  //! valors propers a 0 indiquen que la majoria del blob és intern
  //! valors propers a 1 indiquen que la majoria del blob és extern
  //! Class to calculate the ratio between the perimeter and the number of extern pixels
  class CBlobGetExternPerimeterRatio : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      if (blob.Perimeter() != 0)
        return blob.ExternPerimeter() / blob.Perimeter();
      else
        return blob.ExternPerimeter();
    }
    const char *GetNom() const
    {
      return "CBlobGetExternPerimeterRatio";
    }
  };

  //! Classe per calcular el ratio entre el perimetre convex i nombre pixels externs
  //! valors propers a 0 indiquen que la majoria del blob és intern
  //! valors propers a 1 indiquen que la majoria del blob és extern
  //! Class to calculate the ratio between the perimeter and the number of extern pixels
  class CBlobGetExternHullPerimeterRatio : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CBlobGetHullPerimeter getHullPerimeter;
      double hullPerimeter;

      if ((hullPerimeter = getHullPerimeter(blob)) != 0)
        return blob.ExternPerimeter() / hullPerimeter;
      else
        return blob.ExternPerimeter();
    }
    const char *GetNom() const
    {
      return "CBlobGetExternHullPerimeterRatio";
    }
  };

  //! Classe per calcular el centre en el eix X d'un blob
  //! Class to calculate the center in the X direction
  class CBlobGetXCenter : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MinX() + ((blob.MaxX() - blob.MinX()) / 2.0);
    }
    const char *GetNom() const
    {
      return "CBlobGetXCenter";
    }
  };

  //! Classe per calcular el centre en el eix Y d'un blob
  //! Class to calculate the center in the Y direction
  class CBlobGetYCenter : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      return blob.MinY() + ((blob.MaxY() - blob.MinY()) / 2.0);
    }
    const char *GetNom() const
    {
      return "CBlobGetYCenter";
    }
  };

  //! Classe per calcular la longitud de l'eix major d'un blob
  //! Class to calculate the length of the major axis of the ellipse that fits the blob edges
  class CBlobGetMajorAxisLength : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CvBox2D elipse = blob.GetEllipse();

      return elipse.size.width;
    }
    const char *GetNom() const
    {
      return "CBlobGetMajorAxisLength";
    }
  };

  //! Classe per calcular el ratio entre l'area de la elipse i la de la taca
  //! Class 
  class CBlobGetAreaElipseRatio : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      if (blob.Area() == 0.0) return 0.0;

      CvBox2D elipse = blob.GetEllipse();
      double ratioAreaElipseAreaTaca = ((elipse.size.width / 2.0)
        *
        (elipse.size.height / 2.0)
        *CV_PI
        )
        /
        blob.Area();

      return ratioAreaElipseAreaTaca;
    }
    const char *GetNom() const
    {
      return "CBlobGetAreaElipseRatio";
    }
  };

  //! Classe per calcular la longitud de l'eix menor d'un blob
  //! Class to calculate the length of the minor axis of the ellipse that fits the blob edges
  class CBlobGetMinorAxisLength : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CvBox2D elipse = blob.GetEllipse();

      return elipse.size.height;
    }
    const char *GetNom() const
    {
      return "CBlobGetMinorAxisLength";
    }
  };

  //! Classe per calcular l'orientació de l'ellipse del blob en radians
  //! Class to calculate the orientation of the ellipse that fits the blob edges in radians
  class CBlobGetOrientation : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CvBox2D elipse = blob.GetEllipse();

      if (elipse.angle > 180.0)
        return ((elipse.angle - 180.0)* DEGREE2RAD);
      else
        return (elipse.angle * DEGREE2RAD);

    }
    const char *GetNom() const
    {
      return "CBlobGetOrientation";
    }
  };

  //! Classe per calcular el cosinus de l'orientació de l'ellipse del blob
  //! Class to calculate the cosinus of the orientation of the ellipse that fits the blob edges
  class CBlobGetOrientationCos : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CBlobGetOrientation getOrientation;
      return fabs(cos(getOrientation(blob)));
    }
    const char *GetNom() const
    {
      return "CBlobGetOrientationCos";
    }
  };


  //! Classe per calcular el ratio entre l'eix major i menor de la el·lipse
  //! Class to calculate the ratio between both axes of the ellipse
  class CBlobGetAxisRatio : public COperadorBlob
  {
  public:
    double operator()(const CBlob &blob) const
    {
      CvBox2D elipse = blob.GetEllipse();

      return elipse.size.height / elipse.size.width;
    }
    const char *GetNom() const
    {
      return "CBlobGetAxisRatio";
    }
  };


  //! Classe per calcular si un punt cau dins del blob
  //! Class to calculate whether a point is inside a blob
  class CBlobGetXYInside : public COperadorBlob
  {
  public:
    //! Constructor estàndard
    //! Standard constructor
    CBlobGetXYInside()
    {
      m_p = cvPoint(0, 0);
    }
    //! Constructor: indiquem el punt
    //! Constructor: sets the point
    CBlobGetXYInside(CvPoint p)
    {
      m_p = p;
    };
    double operator()(const CBlob &blob) const;
    const char *GetNom() const
    {
      return "CBlobGetXYInside";
    }

  private:
    //! punt que considerem
    //! point to be considered
    CvPoint m_p;
  };

}

#endif //CBLOB_INSPECTA_INCLUDED

