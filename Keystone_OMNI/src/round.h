#pragma once

#ifndef ROUND__H
#define ROUND__H

#include <math.h>

/* round to integer */

inline int iround(double x)
{
      return (int)floor(x + 0.5);
}

inline int lround(double x)
{
      return (long)floor(x + 0.5);
}

/* round number n to d decimal points */

inline double fround(double n, unsigned d)
{
      return floor(n * pow((double)10, (int)d) + .5) / pow((double)10, (int)d);
}

#endif /* ROUND__H */
