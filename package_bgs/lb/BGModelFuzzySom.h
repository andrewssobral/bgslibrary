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
/*  Scene 1.0.1 -- Background subtraction and object tracking for complex environments  
BGModelFuzzySom.h

Copyright (C) 2011 Laurence Bender <lbender@untref.edu.ar>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef BGMODELFUZZYSOM_H
#define BGMODELFUZZYSOM_H

#include "BGModel.h"

namespace lb_library
{
  namespace FuzzyAdaptiveSOM
  {
    // SOM parameters

    const int M = 3;				// width SOM (per pixel)
    const int N = 3;				// height SOM (per pixel)
    const int KERNEL = 3; 	// size Gaussian kernel 

    const bool SPAN_NEIGHBORS = false; // true if update neighborhood spans different pixels			// 
    const int TRAINING_STEPS = 100;			// number of training steps

    const double EPS1 = 100.0; // model match distance during training
    const double EPS2 = 20.0;  // model match distance
    const double C1 = 1.0;     // learning rate during training
    const double C2 = 0.05;    // learning rate

    const double FUZZYEXP = -5.0;
    const double FUZZYTHRESH = 0.8;

    class BGModelFuzzySom : public BGModel
    {
    public:
      BGModelFuzzySom(int width, int height);
      ~BGModelFuzzySom();

      void setBGModelParameter(int id, int value);

    protected:
      int m_widthSOM;
      int m_heightSOM;
      int m_offset;
      int m_pad;
      int m_K;
      int m_TSteps;

      double m_Wmax;

      double m_epsilon1;
      double m_epsilon2;
      double m_alpha1;
      double m_alpha2;

      DBLRGB** m_ppSOM;					// SOM grid
      double** m_ppW;						// Weights 

      void Init();
      void Update();
    };
  }
}

#endif
