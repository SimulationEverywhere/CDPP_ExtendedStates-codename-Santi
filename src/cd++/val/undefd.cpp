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

/** include files **/
#include "undefd.h"

using namespace std;


#ifdef __WINDOWS__

int ISNAN(double x)
{
	return _isnan(x);
}

long FPCLASS(double x)
{
	return _fpclass(x);
}

#endif
