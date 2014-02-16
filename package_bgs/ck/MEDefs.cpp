/*
 *  This file is part of the AiBO+ project
 *
 *  Copyright (C) 2005-2013 Csaba Kertész (csaba.kertesz@gmail.com)
 *
 *  AiBO+ is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  AiBO+ is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Street #330, Boston, MA 02111-1307, USA.
 *
 */

#include "MEDefs.hpp"

#include <math.h>

float MERound(float number)
{
	double FracPart = 0.0;
	double IntPart = 0.0;
	float Ret = 0.0;

	FracPart = modf((double)number, &IntPart);
	if (number >= 0)
	{
	  Ret = (float)(FracPart >= 0.5 ? IntPart+1 : IntPart);
	} else {
	  Ret = (float)(FracPart <= -0.5 ? IntPart-1 : IntPart);
	}
	return Ret;
}
