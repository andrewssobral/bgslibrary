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
/****************************************************************************
*
* Bgs.hpp
*
* Purpose: Base class for BGS algorithms.
*
* Author: Donovan Parks, October 2007
*
******************************************************************************/

#ifndef BGS_H_
#define BGS_H_

#include "Image.h"
#include "BgsParams.h"

namespace Algorithms
{
	namespace BackgroundSubtraction
	{
		class Bgs
		{
		public:
			static const int BACKGROUND = 0;
			static const int FOREGROUND = 255;

			virtual ~Bgs() {}

			// Initialize any data required by the BGS algorithm. Should be called once before calling
			// any of the following functions.
			virtual void Initalize(const BgsParams& param) = 0;

			// Initialize the background model. Typically, the background model is initialized using the first
			// frame of the incoming video stream, but alternatives are possible.
			virtual void InitModel(const RgbImage& data) = 0;

			// Subtract the current frame from the background model and produce a binary foreground mask using
			// both a low and high threshold value.
			virtual void Subtract(int frame_num, const RgbImage& data,  
															BwImage& low_threshold_mask, BwImage& high_threshold_mask) = 0;	

			// Update the background model. Only pixels set to background in update_mask are updated.
			virtual void Update(int frame_num, const RgbImage& data,  const BwImage& update_mask) = 0;

			// Return the current background model.
			virtual RgbImage *Background() = 0;
		};
	}
}

#endif
