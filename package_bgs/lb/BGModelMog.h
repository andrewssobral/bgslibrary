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
BGModelMog.h

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

#ifndef BGMODELMOGRGB_H
#define BGMODELMOGRGB_H

#include "BGModel.h"

namespace lb_library
{
  namespace MixtureOfGaussians
  {
    const unsigned int NUMBERGAUSSIANS = 3;
    const float LEARNINGRATEMOG = 0.001f;
    const float THRESHOLDMOG = 2.5f;
    const float BGTHRESHOLDMOG = 0.5f;
    const float INITIALVARMOG = 50.0f;

    typedef struct tagMOGDATA
    {
      DBLRGB mu;
      DBLRGB var;
      double w;
      double sortKey;
    } MOGDATA;

    class BGModelMog : public BGModel
    {
    public:
      BGModelMog(int width, int height);
      ~BGModelMog();

      void setBGModelParameter(int id, int value);

    protected:
      double m_alpha;
      double m_threshold;
      double m_noise;
      double m_T;

      MOGDATA* m_pMOG;
      int* m_pK;				// number of distributions per pixel

      void Init();
      void Update();
    };
  }
}

#endif
