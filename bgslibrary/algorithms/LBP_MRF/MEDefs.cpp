#include <math.h>

#include "MEDefs.hpp"

#if CV_MAJOR_VERSION >= 2 && CV_MAJOR_VERSION <= 3

//using namespace bgslibrary::algorithms::lbp_mrf;

namespace bgslibrary
{
  namespace algorithms
  {
    namespace lbp_mrf
    {
      float MERound(float number)
      {
        double FracPart = 0.0;
        double IntPart = 0.0;
        float Ret = 0.0;

        FracPart = modf((double)number, &IntPart);
        if (number >= 0)
          Ret = (float)(FracPart >= 0.5 ? IntPart + 1 : IntPart);
        else
          Ret = (float)(FracPart <= -0.5 ? IntPart - 1 : IntPart);

        return Ret;
      }
    }
  }
}

#endif
