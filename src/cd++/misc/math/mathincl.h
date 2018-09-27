/*******************************************************************
*
*  DESCRIPTION: Math Include
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/02/1999 (v2)
*
*******************************************************************/

#ifndef __MATH_INCLUDE_H
#define __MATH_INCLUDE_H

/** include files **/
#include <math.h>
#include <float.h>

/** my include files **/
#include "value.h"

/** forward declarations **/

#if !defined(M_PI)
#define M_PI	3.14159265358979323846
#endif

#if !defined(M_E)
#define M_E	2.7182818284590452354
#endif

#define M_INFINITO	DBL_MAX
#define M_MINFINITO	(-DBL_MAX)

#define M_GOLDEN	((1 + sqrt(5)) / 2)
#define M_CATALAN	0.9159655941772
#define M_RYDBERG	10973731.534
#define M_GRAV		6.67258e-11
#define M_BOHR_RADIUS	0.529177249e-10
#define M_BOHR_MAGNETON 9.2740154e-24
#define M_BOLTZMANN	1.380658e-23
#define M_ACCEL		9.80665
#define M_LIGHT		299792458
#define	M_ELECTRON_CHARGE	1.60217733e-19
#define M_ELECTRON_MASS	9.1093898e-28
#define M_PROTON_MASS	1.6726231e-24
#define M_NEUTRON_MASS	1.6749286e-24
#define M_PLANCK	6.6260755e-34
#define	M_AVOGADRO	6.0221367e23
#define M_AMU		1.6605402e-27
#define M_PEM		1836.152701
#define M_IDEAL_GAS	22.41410
#define M_FARADAY	96485.309
#define M_STEFAN_BOLTZMANN	5.67051e-8
#define M_EULER_GAMMA	0.577215664901532860606512


inline
Value sign( Value x)
{
	return x < 0 ? -1 : (x > 0 ? 1 : 0);
}


#endif // __MATH_INCLUDE_H
