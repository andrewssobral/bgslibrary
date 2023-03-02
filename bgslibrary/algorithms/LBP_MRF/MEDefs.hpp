#pragma once

#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3 && CV_MINOR_VERSION <= 4 && CV_VERSION_REVISION <= 7

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lbp_mrf
    {
      const double ME_PI_VALUE = 3.14159265;
      
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
    }
  }
}

#endif
