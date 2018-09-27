/*******************************************************************
*
*  DESCRIPTION: Type UNDEFINED
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 2/03/1999 (v2)
*
*******************************************************************/

#ifndef __UNDEFINED_H
#define __UNDEFINED_H

/** include files **/
#include <cmath>
#include <cfloat>

#ifdef __SOLARIS__
	#include <nan.h>
#endif


/** forward declarations **/

#if !defined(NAN)
#define	NAN	(sin(1e100))
#endif

///////////////////////////////////////////////////////////////////////////////
#if !defined(REAL_VALUE_ISNAN)
	#ifdef __WINDOWS__
		int ISNAN(double x);
	
		#define REAL_VALUE_ISNAN(x)	ISNAN(x)
	#else
		// For Unix
		//extern int isnan(double);
		#define REAL_VALUE_ISNAN(x)	std::isnan(x)
	#endif
#endif	// REAL_VALUE_ISNAN
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// isInfinite(x) devuelve 1 si x es infinito, -1 si es -infinito, y 0 en otro caso
#if !defined(isInfinite)
	#ifdef __WINDOWS__

		#if !defined( _FPCLASS_PINF )
			#define _FPCLASS_PINF    0x0200
		#endif

		#if !defined( _FPCLASS_NINF )
			#define _FPCLASS_NINF    0x0004
		#endif

		long FPCLASS(double x);
		
		#define isInfinite(x)	(_fpclass(x)==_FPCLASS_PINF ? 1 : (_fpclass(x)==_FPCLASS_NINF ? -1 : 0))

	#else
		#ifdef __SOLARIS__
			#define isInfinite(x)	(IsNANorINF(x) && IsINF(x))
		#else	// For Unix
			//extern int isinf(double);
			#define isInfinite(x)	std::isinf(x)
		#endif
	#endif
#endif	// REAL_VALUE_ISINF
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
#define UNDEFINED		NAN
#define _isUndefined(x)		REAL_VALUE_ISNAN(x)


#endif // __UNDEFINED_H
