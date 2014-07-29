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
* BgsParams.hpp
*
* Purpose: Base class for BGS parameters. Any parameters common to all BGS
*					 algorithms should be specified directly in this class.
*
* Author: Donovan Parks, May 2008
*
******************************************************************************/

#ifndef BGS_PARAMS_H_
#define BGS_PARAMS_H_

namespace Algorithms
{
	namespace BackgroundSubtraction
	{
		class BgsParams
		{
		public:
			virtual ~BgsParams() {}

			virtual void SetFrameSize(unsigned int width, unsigned int height)
			{
				m_width = width;
				m_height = height;
				m_size = width*height;
			}

			unsigned int &Width() { return m_width; }
			unsigned int &Height() { return m_height; }
			unsigned int &Size() { return m_size; }

		protected:
			unsigned int m_width;
			unsigned int m_height;
			unsigned int m_size;
		};
	}
}

#endif
