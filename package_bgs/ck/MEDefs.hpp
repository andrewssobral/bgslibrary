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

#ifndef MEDefs_hpp
#define MEDefs_hpp

/**
 *  @addtogroup mindeye
 *  @{
 */

/// Pi value
#ifndef ME_PI_VALUE
#define ME_PI_VALUE 3.14159265
#endif

/*! Process state */
typedef enum {
  ps_Min = 0,                /*!< Minimum value */
  ps_Uninitialized = ps_Min, /*!< Uninitialized state */
  ps_Initialized,            /*!< Initialized state */
  ps_InProgress,             /*!< In progress state */
  ps_Successful,             /*!< Successful state */
  ps_Max = ps_Successful     /*!< Maximum value */
} MEProcessStateType;

template <typename T>
const T& MEMin(const T& a, const T& b)
{
  if (a < b)
    return a;
  return b;
}

template <typename T>
const T& MEMax(const T& a, const T& b)
{
  if (a < b)
    return b;
  return a;
}

template <typename T>
const T& MEBound(const T& min, const T& val, const T& max)
{
  return MEMax(min, MEMin(max, val));
}

/*!
 * @brief Round a float number
 *
 * @param number number to round
 *
 * @return New float number
 *
 * This method rounds a float number, if the fraction is .5 or lower
 * then it rounds down, otherwise up.
 *
 */

float MERound(float number);

/** @} */

#endif
