#ifndef __QSS_UTILS_H__
#define __QSS_UTILS_H__

#include "VTime.h"


VTime minposroot(double *coeff, int order);
VTime minposroot_1(double *coeff);
VTime minposroot_2(double *coeff);
VTime minposroot_3(double *coeff);

void advance_time(double *p, const VTime &dt, int order);

#endif
