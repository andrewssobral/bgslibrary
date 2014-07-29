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
//***********************************************************//
//* Blob analysis package  8 August 2003                    *//
//* Version 1.0                                             *//
//* Input: IplImage* binary image                           *//
//* Output: attributes of each connected region             *//
//* Author: Dave Grossman                                   *//
//* Modifications: Francesc Pinyol and Ricard Borras		*//
//* Email: dgrossman@cdr.stanford.edu                       *//
//* Email: fpinyol@cvc.uab.es rborras@cvc.uab.es            *//
//* Acknowledgement: the algorithm has been around > 20 yrs *//
//***********************************************************//

//! Indica si la connectivitat es a 8 (si es desactiva es a 4)
#define B_CONNECTIVITAT_8

//! si la imatge és cíclica verticalment (els blobs que toquen
//! les vores superior i inferior no es consideren externs)
#define IMATGE_CICLICA_VERTICAL		1
//! si la imatge és cíclica horitzontalment (els blobs que toquen
//! les vores dreta i esquerra no es consideren externs)
#define IMATGE_CICLICA_HORITZONTAL	0

#define PERIMETRE_DIAGONAL (1.41421356237310 - 2)
#define SQRT2	1.41421356237310
// color dels píxels de la màscara per ser exteriors
#define PIXEL_EXTERIOR 0


#include "BlobResult.h"
#include "BlobExtraction.h"
#include <opencv2/legacy/compat.hpp>

namespace Blob
{

  /**
  - FUNCIÓ: BlobAnalysis
  - FUNCIONALITAT: Extreu els blobs d'una imatge d'un sol canal
  - PARÀMETRES:
  - inputImage: Imatge d'entrada. Ha de ser d'un sol canal
  - threshold: Nivell de gris per considerar un pixel blanc o negre
  - maskImage: Imatge de màscara fora de la cual no es calculen els blobs. A més,
  els blobs que toquen els pixels de la màscara a 0, són considerats
  externs
  - borderColor: Color del marc de la imatge (0=black or 1=white)
  - findmoments: calcula els moments dels blobs o no
  - RegionData: on es desarà el resultat
  - RESULTAT:
  - retorna true si tot ha anat bé, false si no. Deixa el resultat a blobs.
  - RESTRICCIONS:
  - La imatge d'entrada ha de ser d'un sol canal
  - AUTOR: dgrossman@cdr.stanford.edu
  - DATA DE CREACIÓ: 25-05-2005.
  - MODIFICACIÓ: Data. Autor. Descripció.
  - fpinyol@cvc.uab.es, rborras@cvc.uab.es: adaptació a les OpenCV
  */
  bool BlobAnalysis(IplImage* inputImage,
    uchar threshold,
    IplImage* maskImage,
    bool borderColor,
    bool findmoments,
    blob_vector &RegionData)
  {
    // dimensions of input image taking in account the ROI
    int Cols, Rows, startCol, startRow;

    if (inputImage->roi)
    {
      CvRect imageRoi = cvGetImageROI(inputImage);
      startCol = imageRoi.x;
      startRow = imageRoi.y;
      Cols = imageRoi.width;
      Rows = imageRoi.height;
    }
    else
    {
      startCol = 0;
      startRow = 0;
      Cols = inputImage->width;
      Rows = inputImage->height;
    }

    int Trans = Cols;				// MAX trans in any row
    char* pMask = NULL;
    char* pImage;

    // Convert image array into transition array. In each row
    // the transition array tells which columns have a color change
    int iCol, iRow, iTran, Tran;				// Data for a given run
    bool ThisCell, LastCell;		// Contents (colors (0 or 1)) within this row
    int TransitionOffset = 0;		// Performance booster to avoid multiplication

    // row 0 and row Rows+1 represent the border
    int i;
    int *Transition;				// Transition Matrix

    int nombre_pixels_mascara = 0;
    //! Imatge amb el perimetre extern de cada pixel
    IplImage *imatgePerimetreExtern;

    // input images must have only 1-channel and be an image
    if (!CV_IS_IMAGE(inputImage) || (inputImage->nChannels != 1))
    {
      return false;
    }
    if (maskImage != NULL)
    {
      // input image and mask are a valid image?
      if (!CV_IS_IMAGE(inputImage) || !CV_IS_IMAGE(maskImage))
        return false;

      // comprova que la màscara tingui les mateixes dimensions que la imatge
      if (inputImage->width != maskImage->width || inputImage->height != maskImage->height)
      {
        return false;
      }

      // comprova que la màscara sigui una imatge d'un sol canal (grayscale)
      if (maskImage->nChannels != 1)
      {
        return false;
      }

    }

    // Initialize Transition array
    Transition = new int[(Rows + 2)*(Cols + 2)];
    memset(Transition, 0, (Rows + 2) * (Cols + 2)*sizeof(int));
    Transition[0] = Transition[(Rows + 1) * (Cols + 2)] = Cols + 2;

    // Start at the beginning of the image (startCol, startRow)
    pImage = inputImage->imageData + startCol - 1 + startRow * inputImage->widthStep;

    /*
    Paral·lelització del càlcul de la matriu de transicions
    Fem que cada iteració del for el faci un thread o l'altre ( tenim 2 possibles threads )
    */
    if (maskImage == NULL)
    {
      imatgePerimetreExtern = NULL;

      //Fill Transition array
      for (iRow = 1; iRow < Rows + 1; iRow++)		// Choose a row of Bordered image
      {
        TransitionOffset = iRow*(Cols + 2); //per a que sigui paral·litzable
        iTran = 0;					// Index into Transition array
        Tran = 0;					// No transitions at row start
        LastCell = borderColor;

        for (iCol = 0; iCol < Cols + 2; iCol++)	// Scan that row of Bordered image
        {
          if (iCol == 0 || iCol == Cols + 1)
            ThisCell = borderColor;
          else
            ThisCell = ((unsigned char)*(pImage)) > threshold;

          if (ThisCell != LastCell)
          {
            Transition[TransitionOffset + iTran] = Tran;	// Save completed Tran
            iTran++;						// Prepare new index
            LastCell = ThisCell;			// With this color
          }

          Tran++;	// Tran continues
          pImage++;
        }

        Transition[TransitionOffset + iTran] = Tran;	// Save completed run
        if ((TransitionOffset + iTran + 1) < (Rows + 1)*(Cols + 2))
        {
          Transition[TransitionOffset + iTran + 1] = -1;
        }
        //jump to next row (beginning from (startCol, startRow))
        pImage = inputImage->imageData - 1 + startCol + (iRow + startRow)*inputImage->widthStep;
      }
    }
    else
    {
      //maskImage not NULL: Cal recòrrer la màscara també per calcular la matriu de transicions

      char perimeter;
      char *pPerimetre;

      // creem la imatge que contindrà el perimetre extern de cada pixel
      imatgePerimetreExtern = cvCreateImage(cvSize(maskImage->width, maskImage->height), IPL_DEPTH_8U, 1);
      cvSetZero(imatgePerimetreExtern);

      pMask = maskImage->imageData - 1;

      //Fill Transition array
      for (iRow = 1; iRow < Rows + 1; iRow++)		// Choose a row of Bordered image
      {
        TransitionOffset = iRow*(Cols + 2);
        iTran = 0;					// Index into Transition array
        Tran = 0;					// No transitions at row start
        LastCell = borderColor;

        pPerimetre = imatgePerimetreExtern->imageData + (iRow - 1) * imatgePerimetreExtern->widthStep;
        //pMask = maskImage->imageData + (iRow-1) * maskImage->widthStep;

        for (iCol = 0; iCol < Cols + 2; iCol++)	// Scan that row of Bordered image
        {
          if (iCol == 0 || iCol == Cols + 1 || ((unsigned char)*pMask) == PIXEL_EXTERIOR)
            ThisCell = borderColor;
          else
            ThisCell = ((unsigned char)*(pImage)) > threshold;

          if (ThisCell != LastCell)
          {
            Transition[TransitionOffset + iTran] = Tran;	// Save completed Tran
            iTran++;						// Prepare new index
            LastCell = ThisCell;			// With this color
          }

          /*////////////////////////////////////////////////////////////////////////
          Calcul de la imatge amb els pixels externs
          ////////////////////////////////////////////////////////////////////////*/
          // pels pixels externs no cal calcular res pq no hi accedir-hem
          if ((iCol > 0) && (iCol < Cols))
          {
            if (*pMask == PIXEL_EXTERIOR)
            {
              *pPerimetre = 0;
            }
            else
            {
              perimeter = 0;

              // pixels al nord de l'actual
              if (iRow > 1)
              {
                if (*(pMask - maskImage->widthStep) == PIXEL_EXTERIOR) perimeter++;
              }

              // pixels a l'est i oest de l'actual
              if (iRow < imatgePerimetreExtern->height)
              {
                if ((iCol>0) && (*(pMask - 1) == PIXEL_EXTERIOR)) perimeter++;

                if ((iCol < imatgePerimetreExtern->width - 1) && (*(pMask + 1) == PIXEL_EXTERIOR)) perimeter++;
              }

              // pixels al sud de l'actual
              if (iRow < imatgePerimetreExtern->height - 1)
              {
                if ((*(pMask + maskImage->widthStep) == PIXEL_EXTERIOR)) perimeter++;
              }

              *pPerimetre = perimeter;
            }
          }

          Tran++;	// Tran continues
          pImage++;
          pMask++;
          pPerimetre++;
        }
        Transition[TransitionOffset + iTran] = Tran;	// Save completed run

        if ((TransitionOffset + iTran + 1) < (Rows + 1)*(Cols + 2))
        {
          Transition[TransitionOffset + iTran + 1] = -1;
        }


        //jump to next row (beginning from (startCol, startRow))
        pImage = inputImage->imageData - 1 + startCol + (iRow + startRow)*inputImage->widthStep;
        //the mask should be the same size as image Roi, so don't take into account the offset
        pMask = maskImage->imageData - 1 + iRow*maskImage->widthStep;
      }
    }

    // Process transition code depending on Last row and This row
    //
    // Last ---++++++--+++++++++++++++-----+++++++++++++++++++-----++++++-------+++---
    // This -----+++-----++++----+++++++++----+++++++---++------------------++++++++--
    //
    // There are various possibilities:
    //
    // Case     1       2       3       4       5       6       7       8
    // Last |xxx    |xxxxoo |xxxxxxx|xxxxxxx|ooxxxxx|ooxxx  |ooxxxxx|    xxx|
    // This |    yyy|    yyy|  yyyy |  yyyyy|yyyyyyy|yyyyyyy|yyyy   |yyyy   |
    // Here o is optional
    // 
    // Here are the primitive tests to distinguish these 6 cases:
    //   A) Last end < This start - 1 OR NOT		Note: -1
    //   B) This end < Last start OR NOT
    //   C) Last start < This start OR NOT
    //   D) This end < Last end OR NOT
    //   E) This end = Last end OR NOT
    //
    // Here is how to use these tests to determine the case:
    //   Case 1 = A [=> NOT B AND C AND NOT D AND NOT E]
    //   Case 2 = C AND NOT D AND NOT E [AND NOT A AND NOT B]
    //   Case 3 = C AND D [=> NOT E] [AND NOT A AND NOT B]
    //   Case 4 = C AND NOT D AND E [AND NOT A AND NOT B]
    //   Case 5 = NOT C AND E [=> NOT D] [AND NOT A AND NOT B]
    //   Case 6 = NOT C AND NOT D AND NOT E [AND NOT A AND NOT B]
    //   Case 7 = NOT C AND D [=> NOT E] [AND NOT A AND NOT B]
    //   Case 8 = B [=> NOT A AND NOT C AND D AND NOT E]
    //
    // In cases 2,3,4,5,6,7 the following additional test is needed:
    //   Match) This color = Last color OR NOT
    //
    // In cases 5,6,7 the following additional test is needed:
    //   Known) This region was already matched OR NOT
    //
    // Here are the main tests and actions:
    //   Case 1: LastIndex++;
    //   Case 2: if(Match) {y = x;}
    //           LastIndex++;
    //   Case 3: if(Match) {y = x;}
    //           else {y = new}
    //           ThisIndex++;
    //   Case 4: if(Match) {y = x;}
    //           else {y = new}
    //           LastIndex++;
    //           ThisIndex++;
    //   Case 5: if(Match AND NOT Known) {y = x}
    //           else if(Match AND Known) {Subsume(x,y)}
    //           LastIndex++;ThisIndex++
    //   Case 6: if(Match AND NOT Known) {y = x}
    //           else if(Match AND Known) {Subsume(x,y)}
    //           LastIndex++;
    //   Case 7: if(Match AND NOT Known) {y = x}
    //           else if(Match AND Known) {Subsume(x,y)}
    //           ThisIndex++;
    //   Case 8: ThisIndex++;

    int *SubsumedRegion = NULL;

    double ThisParent;	// These data can change when the line is current
    double ThisArea;
    double ThisPerimeter;
    double ThisSumX = 0;
    double ThisSumY = 0;
    double ThisSumXX = 0;
    double ThisSumYY = 0;
    double ThisSumXY = 0;
    double ThisMinX;
    double ThisMaxX;
    double ThisMinY;
    double ThisMaxY;
    double LastPerimeter;	// This is the only data for retroactive change
    double ThisExternPerimeter;

    int HighRegionNum = 0;
    //int RegionNum = 0;
    int ErrorFlag = 0;

    int LastRow, ThisRow;			// Row number
    int LastStart, ThisStart;		// Starting column of run
    int LastEnd, ThisEnd;			// Ending column of run
    int LastColor, ThisColor;		// Color of run

    int LastIndex, ThisIndex;		// Which run are we up to
    int LastIndexCount, ThisIndexCount;	// Out of these runs
    int LastRegionNum, ThisRegionNum;	// Which assignment
    int *LastRegion;				// Row assignment of region number
    int *ThisRegion;		// Row assignment of region number

    int LastOffset = -(Trans + 2);	// For performance to avoid multiplication
    int ThisOffset = 0;				// For performance to avoid multiplication
    int ComputeData;

    CvPoint actualedge;
    uchar imagevalue;
    bool CandidatExterior = false;

    // apuntadors als blobs de la regió actual i last
    CBlob *regionDataThisRegion, *regionDataLastRegion;

    LastRegion = new int[Cols + 2];
    ThisRegion = new int[Cols + 2];

    for (i = 0; i < Cols + 2; i++)	// Initialize result arrays
    {
      LastRegion[i] = -1;
      ThisRegion[i] = -1;
    }

    //create the external blob
    RegionData.push_back(new CBlob());
    SubsumedRegion = NewSubsume(SubsumedRegion, 0);
    RegionData[0]->parent = -1;
    RegionData[0]->area = (double)Transition[0];
    RegionData[0]->perimeter = (double)(2 + 2 * Transition[0]);

    ThisIndexCount = 1;
    ThisRegion[0] = 0;	// Border region

    // beginning of the image 
    // en cada linia, pimage apunta al primer pixel de la fila
    pImage = inputImage->imageData - 1 + startCol + startRow * inputImage->widthStep;
    //the mask should be the same size as image Roi, so don't take into account the offset
    if (maskImage != NULL) pMask = maskImage->imageData - 1;

    char *pImageAux, *pMaskAux = NULL;

    // Loop over all rows
    for (ThisRow = 1; ThisRow < Rows + 2; ThisRow++)
    {
      //cout << "========= THIS ROW = " << ThisRow << endl;	// for debugging
      ThisOffset += Trans + 2;
      ThisIndex = 0;
      LastOffset += Trans + 2;;
      LastRow = ThisRow - 1;
      LastIndexCount = ThisIndexCount;
      LastIndex = 0;

      int EndLast = 0;
      int EndThis = 0;

      for (int j = 0; j < Trans + 2; j++)
      {
        int Index = ThisOffset + j;
        int TranVal = Transition[Index];
        if (TranVal > 0) ThisIndexCount = j + 1;	// stop at highest 

        if (ThisRegion[j] == -1)  { EndLast = 1; }
        if (TranVal < 0) { EndThis = 1; }

        if (EndLast > 0 && EndThis > 0) { break; }

        LastRegion[j] = ThisRegion[j];
        ThisRegion[j] = -1;		// Flag indicates region is not initialized
      }

      int MaxIndexCount = LastIndexCount;
      if (ThisIndexCount > MaxIndexCount) MaxIndexCount = ThisIndexCount;

      // Main loop over runs within Last and This rows
      while (LastIndex < LastIndexCount && ThisIndex < ThisIndexCount)
      {
        ComputeData = 0;

        if (LastIndex == 0) LastStart = 0;
        else LastStart = Transition[LastOffset + LastIndex - 1];
        LastEnd = Transition[LastOffset + LastIndex] - 1;
        LastColor = LastIndex - 2 * (LastIndex / 2);
        LastRegionNum = LastRegion[LastIndex];

        regionDataLastRegion = RegionData[LastRegionNum];


        if (ThisIndex == 0) ThisStart = 0;
        else ThisStart = Transition[ThisOffset + ThisIndex - 1];
        ThisEnd = Transition[ThisOffset + ThisIndex] - 1;
        ThisColor = ThisIndex - 2 * (ThisIndex / 2);
        ThisRegionNum = ThisRegion[ThisIndex];

        if (ThisRegionNum >= 0)
          regionDataThisRegion = RegionData[ThisRegionNum];
        else
          regionDataThisRegion = NULL;


        // blobs externs
        CandidatExterior = false;
        if (
#if !IMATGE_CICLICA_VERTICAL
          ThisRow == 1 || ThisRow == Rows ||
#endif
#if !IMATGE_CICLICA_HORITZONTAL
          ThisStart <= 1 || ThisEnd >= Cols ||
#endif				
          GetExternPerimeter(ThisStart, ThisEnd, ThisRow, inputImage->width, inputImage->height, imatgePerimetreExtern)
          )
        {
          CandidatExterior = true;
        }

        int TestA = (LastEnd < ThisStart - 1);	// initially false
        int TestB = (ThisEnd < LastStart);		// initially false
        int TestC = (LastStart < ThisStart);	// initially false
        int TestD = (ThisEnd < LastEnd);
        int TestE = (ThisEnd == LastEnd);

        int TestMatch = (ThisColor == LastColor);		// initially true
        int TestKnown = (ThisRegion[ThisIndex] >= 0);	// initially false

        int Case = 0;
        if (TestA) Case = 1;
        else if (TestB) Case = 8;
        else if (TestC)
        {
          if (TestD) Case = 3;
          else if (!TestE) Case = 2;
          else Case = 4;
        }
        else
        {
          if (TestE) Case = 5;
          else if (TestD) Case = 7;
          else Case = 6;
        }

        // Initialize common variables
        ThisArea = (float) 0.0;

        if (findmoments)
        {
          ThisSumX = ThisSumY = (float) 0.0;
          ThisSumXX = ThisSumYY = ThisSumXY = (float) 0.0;
        }
        ThisMinX = ThisMinY = (float) 1000000.0;
        ThisMaxX = ThisMaxY = (float)-1.0;

        LastPerimeter = ThisPerimeter = (float) 0.0;
        ThisParent = (float)-1;
        ThisExternPerimeter = 0.0;

        // Determine necessary action and take it
        switch (Case)
        {
        case 1: //|xxx    |
          //|    yyy|

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          LastIndex++;

          //afegim la cantonada a LastRegion
          actualedge.x = ThisEnd;
          actualedge.y = ThisRow - 1;
          cvSeqPush(regionDataLastRegion->edges, &actualedge);

          //afegim la cantonada a ThisRegion
          actualedge.x = ThisStart - 1;
          actualedge.y = ThisRow - 1;
          cvSeqPush(regionDataThisRegion->edges, &actualedge);

          break;


        case 2: //|xxxxoo |
          //|    yyy|

          if (TestMatch)	// Same color
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;

            ThisArea = ThisEnd - ThisStart + 1;
            LastPerimeter = LastEnd - ThisStart + 1;	// to subtract
            ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter +
              PERIMETRE_DIAGONAL * 2;

            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);
              ThisExternPerimeter += PERIMETRE_DIAGONAL * 2;
            }
            ComputeData = 1;
          }

          //afegim la cantonada a ThisRegion
          if (ThisRegionNum != -1)
          {
            // afegim dos vertexs si són diferents, només
            if (ThisStart - 1 != ThisEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
          }
          //afegim la cantonada a ThisRegion
          if (LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            // afegim dos vertexs si són diferents, només
            if (ThisStart - 1 != ThisEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataLastRegion->edges, &actualedge);
            }
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          LastIndex++;
          break;


        case 3: //|xxxxxxx|
          //|  yyyy |

          if (TestMatch)	// Same color
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;

            ThisArea = ThisEnd - ThisStart + 1;
            LastPerimeter = ThisArea;	// to subtract
            ThisPerimeter = 2 + ThisArea + PERIMETRE_DIAGONAL * 2;
            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);

              ThisExternPerimeter += PERIMETRE_DIAGONAL * 2;
            }
          }
          else		// Different color => New region
          {
            ThisParent = LastRegionNum;
            ThisRegionNum = ++HighRegionNum;
            ThisArea = ThisEnd - ThisStart + 1;
            ThisPerimeter = 2 + 2 * ThisArea;
            RegionData.push_back(new CBlob());
            regionDataThisRegion = RegionData.back();

            SubsumedRegion = NewSubsume(SubsumedRegion, HighRegionNum);
            if (CandidatExterior)
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
              inputImage->width, inputImage->height,
              imatgePerimetreExtern);

          }

          if (ThisRegionNum != -1)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
            //afegim la cantonada a la regio
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
          }
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
            //afegim la cantonada a la regio
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          ComputeData = 1;
          ThisIndex++;
          break;


        case 4:	//|xxxxxxx|
          //|  yyyyy|

          if (TestMatch)	// Same color
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;
            ThisArea = ThisEnd - ThisStart + 1;
            LastPerimeter = ThisArea;	// to subtract
            ThisPerimeter = 2 + ThisArea + PERIMETRE_DIAGONAL;
            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);

              ThisExternPerimeter += PERIMETRE_DIAGONAL;
            }
          }
          else		// Different color => New region
          {
            ThisParent = LastRegionNum;
            ThisRegionNum = ++HighRegionNum;
            ThisArea = ThisEnd - ThisStart + 1;
            ThisPerimeter = 2 + 2 * ThisArea;
            RegionData.push_back(new CBlob());
            regionDataThisRegion = RegionData.back();
            SubsumedRegion = NewSubsume(SubsumedRegion, HighRegionNum);
            if (CandidatExterior)
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
              inputImage->width, inputImage->height,
              imatgePerimetreExtern);

          }

          if (ThisRegionNum != -1)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
          }
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          ComputeData = 1;

#ifdef B_CONNECTIVITAT_8
          if (TestMatch)
          {
            LastIndex++;
            ThisIndex++;
          }
          else
          {
            LastIndex++;
          }
#else
          LastIndex++;
          ThisIndex++;
#endif					
          break;


        case 5:	//|ooxxxxx|
          //|yyyyyyy|

          if (!TestMatch && !TestKnown)	// Different color and unknown => new region
          {
            ThisParent = LastRegionNum;
            ThisRegionNum = ++HighRegionNum;
            ThisArea = ThisEnd - ThisStart + 1;
            ThisPerimeter = 2 + 2 * ThisArea;
            RegionData.push_back(new CBlob());
            regionDataThisRegion = RegionData.back();
            SubsumedRegion = NewSubsume(SubsumedRegion, HighRegionNum);
            if (CandidatExterior)
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
              inputImage->width, inputImage->height,
              imatgePerimetreExtern);

          }
          else if (TestMatch && !TestKnown)	// Same color and unknown
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;
            ThisArea = ThisEnd - ThisStart + 1;
            LastPerimeter = LastEnd - LastStart + 1;	// to subtract
            ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter
              + PERIMETRE_DIAGONAL * (LastStart != ThisStart);
            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);


              ThisExternPerimeter += PERIMETRE_DIAGONAL * (LastStart != ThisStart);
            }
            ComputeData = 1;
          }
          else if (TestMatch && TestKnown)	// Same color and known
          {
            LastPerimeter = LastEnd - LastStart + 1;	// to subtract
            //ThisPerimeter = - LastPerimeter;
            ThisPerimeter = -2 * LastPerimeter
              + PERIMETRE_DIAGONAL * (LastStart != ThisStart);

            if (ThisRegionNum > LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataThisRegion, regionDataLastRegion,
                findmoments, ThisRegionNum, LastRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
                if (LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
              }
              ThisRegionNum = LastRegionNum;
            }
            else if (ThisRegionNum < LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataLastRegion, regionDataThisRegion,
                findmoments, LastRegionNum, ThisRegionNum);

              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
                if (LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
              }
              LastRegionNum = ThisRegionNum;
            }
          }


          if (ThisRegionNum != -1)
          {
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);

            if (ThisStart - 1 != LastEnd)
            {
              //afegim la cantonada a la regio
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
          }
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;

#ifdef B_CONNECTIVITAT_8
          if (TestMatch)
          {
            LastIndex++;
            ThisIndex++;
          }
          else
          {
            LastIndex++;
          }
#else
          LastIndex++;
          ThisIndex++;
#endif	
          break;


        case 6:	//|ooxxx  |
          //|yyyyyyy|

          if (TestMatch && !TestKnown)
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;
            ThisArea = ThisEnd - ThisStart + 1;
            LastPerimeter = LastEnd - LastStart + 1;	// to subtract
            ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter
              + PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);
            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);


              ThisExternPerimeter += PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);
            }
            ComputeData = 1;
          }
          else if (TestMatch && TestKnown)
          {
            LastPerimeter = LastEnd - LastStart + 1;	// to subtract
            //ThisPerimeter = - LastPerimeter;
            ThisPerimeter = -2 * LastPerimeter
              + PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);

            if (ThisRegionNum > LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataThisRegion, regionDataLastRegion,
                findmoments, ThisRegionNum, LastRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
                if (LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
              }
              ThisRegionNum = LastRegionNum;
            }
            else if (ThisRegionNum < LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataLastRegion, regionDataThisRegion,
                findmoments, LastRegionNum, ThisRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
                if (LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
              }
              LastRegionNum = ThisRegionNum;
            }
          }


          if (ThisRegionNum != -1)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
            if (ThisStart - 1 != LastEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
          }
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            //afegim la cantonada a la regio
            if (ThisStart - 1 != LastEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          LastIndex++;
          break;


        case 7:	//|ooxxxxx|
          //|yyyy   |

          if (!TestMatch && !TestKnown)	// Different color and unknown => new region
          {
            ThisParent = LastRegionNum;
            ThisRegionNum = ++HighRegionNum;
            ThisArea = ThisEnd - ThisStart + 1;
            ThisPerimeter = 2 + 2 * ThisArea;
            RegionData.push_back(new CBlob());
            regionDataThisRegion = RegionData.back();
            SubsumedRegion = NewSubsume(SubsumedRegion, HighRegionNum);
            if (CandidatExterior)
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
              inputImage->width, inputImage->height,
              imatgePerimetreExtern);

          }
          else if (TestMatch && !TestKnown)
          {
            ThisRegionNum = LastRegionNum;
            regionDataThisRegion = regionDataLastRegion;
            ThisArea = ThisEnd - ThisStart + 1;
            ThisPerimeter = 2 + ThisArea;
            LastPerimeter = ThisEnd - LastStart + 1;
            ThisPerimeter = 2 + 2 * ThisArea - LastPerimeter
              + PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);
            if (CandidatExterior)
            {
              ThisExternPerimeter = GetExternPerimeter(ThisStart, ThisEnd, ThisRow,
                inputImage->width, inputImage->height,
                imatgePerimetreExtern);

              ThisExternPerimeter += PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);
            }
            ComputeData = 1;
          }
          else if (TestMatch && TestKnown)
          {
            LastPerimeter = ThisEnd - LastStart + 1;	// to subtract
            //ThisPerimeter = - LastPerimeter;
            ThisPerimeter = -2 * LastPerimeter
              + PERIMETRE_DIAGONAL + PERIMETRE_DIAGONAL * (ThisStart != LastStart);

            if (ThisRegionNum > LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataThisRegion, regionDataLastRegion,
                findmoments, ThisRegionNum, LastRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
                if (LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
              }
              ThisRegionNum = LastRegionNum;
            }
            else if (ThisRegionNum < LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataLastRegion, regionDataThisRegion,
                findmoments, LastRegionNum, ThisRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
                if (LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
              }
              LastRegionNum = ThisRegionNum;
            }
          }

          if (ThisRegionNum != -1)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
            if (ThisStart - 1 != LastEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
          }
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisEnd;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
            if (ThisStart - 1 != LastEnd)
            {
              actualedge.x = ThisStart - 1;
              actualedge.y = ThisRow - 1;
              cvSeqPush(regionDataThisRegion->edges, &actualedge);
            }
          }

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          ThisIndex++;
          break;

        case 8:	//|    xxx|
          //|yyyy   |

#ifdef B_CONNECTIVITAT_8					
          // fusionem blobs
          if (TestMatch)
          {
            if (ThisRegionNum > LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataThisRegion, regionDataLastRegion,
                findmoments, ThisRegionNum, LastRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == ThisRegionNum) ThisRegion[iOld] = LastRegionNum;
                if (LastRegion[iOld] == ThisRegionNum) LastRegion[iOld] = LastRegionNum;
              }
              ThisRegionNum = LastRegionNum;
            }
            else if (ThisRegionNum < LastRegionNum)
            {
              Subsume(RegionData, HighRegionNum, SubsumedRegion, regionDataLastRegion, regionDataThisRegion,
                findmoments, LastRegionNum, ThisRegionNum);
              for (int iOld = 0; iOld < MaxIndexCount; iOld++)
              {
                if (ThisRegion[iOld] == LastRegionNum) ThisRegion[iOld] = ThisRegionNum;
                if (LastRegion[iOld] == LastRegionNum) LastRegion[iOld] = ThisRegionNum;
              }
              LastRegionNum = ThisRegionNum;
            }

            regionDataThisRegion->perimeter = regionDataThisRegion->perimeter + PERIMETRE_DIAGONAL * 2;
          }
#endif

          if (ThisRegionNum != -1)
          {
            //afegim la cantonada a la regio
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataThisRegion->edges, &actualedge);
          }
#ifdef B_CONNECTIVITAT_8					
          // si hem creat un nou blob, afegim tb a l'anterior
          if (!TestMatch && LastRegionNum != -1 && LastRegionNum != ThisRegionNum)
          {
#endif					
            //afegim la cantonada a la regio
            actualedge.x = ThisStart - 1;
            actualedge.y = ThisRow - 1;
            cvSeqPush(regionDataLastRegion->edges, &actualedge);
#ifdef B_CONNECTIVITAT_8
          }
#endif

          ThisRegion[ThisIndex] = ThisRegionNum;
          LastRegion[LastIndex] = LastRegionNum;
          ThisIndex++;
#ifdef B_CONNECTIVITAT_8					
          LastIndex--;
#endif
          break;

        default:
          ErrorFlag = -1;
        }	// end switch case

        // calculate the blob moments and mean gray level of the current blob (ThisRegionNum)
        if (ComputeData > 0)
        {
          // compute blob moments if necessary
          if (findmoments)
          {
            float ImageRow = (float)(ThisRow - 1);

            for (int k = ThisStart; k <= ThisEnd; k++)
            {
              ThisSumX += (float)(k - 1);
              ThisSumXX += (float)(k - 1) * (k - 1);
            }

            ThisSumXY = ThisSumX * ImageRow;
            ThisSumY = ThisArea * ImageRow;
            ThisSumYY = ThisSumY * ImageRow;

          }

          // compute the mean gray level and its std deviation 
          if (ThisRow <= Rows)
          {
            pImageAux = pImage + ThisStart;
            if (maskImage != NULL) pMaskAux = pMask + ThisStart;
            for (int k = ThisStart; k <= ThisEnd; k++)
            {
              if ((k > 0) && (k <= Cols))
              {
                if (maskImage != NULL)
                {
                  // només es té en compte el valor del píxel de la
                  // imatge que queda dins de la màscara
                  // (de pas, comptem el nombre de píxels de la màscara)
                  if (((unsigned char)*pMaskAux) != PIXEL_EXTERIOR)
                  {
                    imagevalue = (unsigned char)(*pImageAux);
                    regionDataThisRegion->mean += imagevalue;
                    regionDataThisRegion->stddev += imagevalue*imagevalue;
                  }
                  else
                  {
                    nombre_pixels_mascara++;
                  }
                }
                else
                {
                  imagevalue = (unsigned char)(*pImageAux);
                  regionDataThisRegion->mean += imagevalue;
                  regionDataThisRegion->stddev += imagevalue*imagevalue;

                }
              }
              pImageAux++;
              if (maskImage != NULL) pMaskAux++;
            }
          }

          // compute the min and max values of X and Y
          if (ThisStart - 1 < (int)ThisMinX) ThisMinX = (float)(ThisStart - 1);
          if (ThisMinX < (float) 0.0) ThisMinX = (float) 0.0;
          if (ThisEnd >(int) ThisMaxX) ThisMaxX = (float)ThisEnd;

          if (ThisRow - 1 < ThisMinY) ThisMinY = ThisRow - 1;
          if (ThisMinY < (float) 0.0) ThisMinY = (float) 0.0;
          if (ThisRow > ThisMaxY) ThisMaxY = ThisRow;
        }

        // put the current results into RegionData
        if (ThisRegionNum >= 0)
        {
          if (ThisParent >= 0) { regionDataThisRegion->parent = (int)ThisParent; }
          regionDataThisRegion->etiqueta = ThisRegionNum;
          regionDataThisRegion->area += ThisArea;
          regionDataThisRegion->perimeter += ThisPerimeter;
          regionDataThisRegion->externPerimeter += ThisExternPerimeter;

          if (ComputeData > 0)
          {
            if (findmoments)
            {
              regionDataThisRegion->sumx += ThisSumX;
              regionDataThisRegion->sumy += ThisSumY;
              regionDataThisRegion->sumxx += ThisSumXX;
              regionDataThisRegion->sumyy += ThisSumYY;
              regionDataThisRegion->sumxy += ThisSumXY;
            }
            regionDataThisRegion->perimeter -= LastPerimeter;
            regionDataThisRegion->minx = MIN(regionDataThisRegion->minx, ThisMinX);
            regionDataThisRegion->maxx = MAX(regionDataThisRegion->maxx, ThisMaxX);
            regionDataThisRegion->miny = MIN(regionDataThisRegion->miny, ThisMinY);
            regionDataThisRegion->maxy = MAX(regionDataThisRegion->maxy, ThisMaxY);
          }
          // blobs externs
          if (CandidatExterior)
          {
            regionDataThisRegion->exterior = true;
          }

        }
      }	// end Main loop

      if (ErrorFlag != 0) {
          delete[] Transition;
          delete[] ThisRegion;
          delete[] LastRegion;
          return false;
      }
      // ens situem al primer pixel de la seguent fila
      pImage = inputImage->imageData - 1 + startCol + (ThisRow + startRow) * inputImage->widthStep;

      if (maskImage != NULL)
        pMask = maskImage->imageData - 1 + ThisRow * maskImage->widthStep;
    }	// end Loop over all rows

    // eliminem l'àrea del marc
    // i també els píxels de la màscara
    // ATENCIO: PERFER: el fet de restar el nombre_pixels_mascara del
    // blob 0 només serà cert si la màscara té contacte amb el marc.
    // Si no, s'haurà de trobar quin és el blob que conté més píxels del
    // compte.
    RegionData[0]->area -= (Rows + 1 + Cols + 1) * 2 + nombre_pixels_mascara;

    // eliminem el perímetre de més:
    // - sense marc: 2m+2n (perímetre extern)
    // - amb marc:   2(m+2)+2(n+2) = 2m+2n + 8
    // (segurament no és del tot acurat)
    // (i amb les màscares encara menys...)
    RegionData[0]->perimeter -= 8.0;

    // Condense the list
    blob_vector::iterator itNew, itOld, iti;
    CBlob *blobActual;

    itNew = RegionData.begin();
    itOld = RegionData.begin();
    int iNew = 0;
    for (int iOld = 0; iOld <= HighRegionNum; iOld++, itOld++)
    {
      if (SubsumedRegion[iOld] < 1)	// This number not subsumed
      {
        // Move data from old region number to new region number
        //*RegionData[iNew] = *RegionData[iOld];
        **itNew = **itOld;

        // Update and parent pointer if necessary
        iti = RegionData.begin();
        for (int i = 0; i <= HighRegionNum; i++)
        {
          //if(RegionData[i]->parent == iOld) { RegionData[i]->parent = iNew; }
          if ((*iti)->parent == iOld) { (*iti)->parent = iNew; }

          ++iti;
        }
        iNew++;
        ++itNew;
      }
    }


    HighRegionNum = iNew - 1;				// Update where the data ends
    RegionData[HighRegionNum]->parent = -1;	// and set end of array flag


    if (findmoments)
    {
      iti = RegionData.begin();
      // Normalize summation fields into moments 
      for (ThisRegionNum = 0; ThisRegionNum <= HighRegionNum; ThisRegionNum++, iti++)
      {
        blobActual = *iti;

        // Get averages
        blobActual->sumx /= blobActual->area;
        blobActual->sumy /= blobActual->area;
        blobActual->sumxx /= blobActual->area;
        blobActual->sumyy /= blobActual->area;
        blobActual->sumxy /= blobActual->area;

        // Create moments
        blobActual->sumxx -= blobActual->sumx * blobActual->sumx;
        blobActual->sumyy -= blobActual->sumy * blobActual->sumy;
        blobActual->sumxy -= blobActual->sumx * blobActual->sumy;
        if (blobActual->sumxy > -1.0E-14 && blobActual->sumxy < 1.0E-14)
        {
          blobActual->sumxy = (float) 0.0; // Eliminate roundoff error
        }

      }
    }

    //Get the real mean and std deviation
    iti = RegionData.begin();
    for (ThisRegionNum = 0; ThisRegionNum <= HighRegionNum; ThisRegionNum++, iti++)
    {
      blobActual = *iti;
      if (blobActual->area > 1)
      {
        blobActual->stddev =
          sqrt(
          (
          blobActual->stddev * blobActual->area -
          blobActual->mean * blobActual->mean
          ) /
          (blobActual->area*(blobActual->area - 1))
          );
      }
      else
        blobActual->stddev = 0;

      if (blobActual->area > 0)
        blobActual->mean /= blobActual->area;
      else
        blobActual->mean = 0;

    }
    // eliminem els blobs subsumats
    blob_vector::iterator itBlobs = RegionData.begin() + HighRegionNum + 1;
    while (itBlobs != RegionData.end())
    {
      delete *itBlobs;
      //RegionData.erase( itBlobs );
      ++itBlobs;
    }
    RegionData.erase(RegionData.begin() + HighRegionNum + 1, RegionData.end());

    //free(RegionData);
    free(SubsumedRegion);
    delete[] Transition;
    delete[] ThisRegion;
    delete[] LastRegion;

    if (imatgePerimetreExtern) cvReleaseImage(&imatgePerimetreExtern);

    return true;
  }


  int *NewSubsume(int *subsumed, int index_subsume)
  {
    if (index_subsume == 0)
    {
      subsumed = (int*)malloc(sizeof(int));
    }
    else
    {
      subsumed = (int*)realloc(subsumed, (index_subsume + 1)*sizeof(int));
    }
    subsumed[index_subsume] = 0;
    return subsumed;
  }

  /**
  Fusiona dos blobs i afegeix el blob les característiques del blob RegionData[HiNum]
  al blob RegionData[LoNum]. Al final allibera el blob de RegionData[HiNum]
  */
  void Subsume(blob_vector &RegionData,
    int HighRegionNum,
    int* SubsumedRegion,
    CBlob* blobHi,
    CBlob* blobLo,
    bool findmoments,
    int HiNum,
    int LoNum)
  {
    // cout << "\nSubsuming " << HiNum << " into " << LoNum << endl; // for debugging

    int i;

    blobLo->minx = MIN(blobHi->minx, blobLo->minx);
    blobLo->miny = MIN(blobHi->miny, blobLo->miny);
    blobLo->maxx = MAX(blobHi->maxx, blobLo->maxx);
    blobLo->maxy = MAX(blobHi->maxy, blobLo->maxy);
    blobLo->area += blobHi->area;
    blobLo->perimeter += blobHi->perimeter;
    blobLo->externPerimeter += blobHi->externPerimeter;
    blobLo->exterior = blobLo->exterior || blobHi->exterior;
    blobLo->mean += blobHi->mean;
    blobLo->stddev += blobHi->stddev;

    if (findmoments)
    {
      blobLo->sumx += blobHi->sumx;
      blobLo->sumy += blobHi->sumy;
      blobLo->sumxx += blobHi->sumxx;
      blobLo->sumyy += blobHi->sumyy;
      blobLo->sumxy += blobHi->sumxy;
    }
    // Make sure no region still has subsumed region as parent
    blob_vector::iterator it = (RegionData.begin() + HiNum + 1);

    for (i = HiNum + 1; i <= HighRegionNum; i++, it++)
    {
      if ((*it)->parent == (float)HiNum) { (*it)->parent = LoNum; }
    }

    // Mark dead region number for future compression
    SubsumedRegion[HiNum] = 1;
    // marquem el blob com a lliure
    blobHi->etiqueta = -1;

    // Atenció!!!! abans d'eliminar els edges 
    // s'han de traspassar del blob HiNum al blob LoNum
    blobHi->CopyEdges(*blobLo);
    blobHi->ClearEdges();
  }

  /**
  - FUNCIÓ: GetExternPerimeter
  - FUNCIONALITAT: Retorna el perimetre extern d'una run lenght
  - PARÀMETRES:
  - start: columna d'inici del run
  - end: columna final del run
  - row: fila del run
  - maskImage: màscara pels pixels externs
  - RESULTAT:
  - quantitat de perimetre extern d'un run, suposant que és un blob
  d'una única fila d'alçada
  - RESTRICCIONS:
  - AUTOR:
  - DATA DE CREACIÓ: 2006/02/27
  - MODIFICACIÓ: Data. Autor. Descripció.
  */
  double GetExternPerimeter(int start, int end, int row, int width, int height, IplImage *imatgePerimetreExtern)
  {
    double perimeter = 0.0f;
    char *pPerimetre;


    // comprovem les dimensions de la imatge
    perimeter += (start <= 0) + (end >= width - 1);
    if (row <= 1) perimeter += start - end;
    if (row >= height - 1) perimeter += start - end;


    // comprovem els pixels que toquen a la màscara (si s'escau)
    if (imatgePerimetreExtern != NULL)
    {
      if (row <= 0 || row >= height) return perimeter;

      if (start < 0) start = 1;
      if (end >= width) end = width - 2;

      pPerimetre = imatgePerimetreExtern->imageData + (row - 1) * imatgePerimetreExtern->widthStep + start;
      for (int x = start - 1; x <= end; x++)
      {
        perimeter += *pPerimetre;
        pPerimetre++;
      }
    }

    return perimeter;
  }

}
