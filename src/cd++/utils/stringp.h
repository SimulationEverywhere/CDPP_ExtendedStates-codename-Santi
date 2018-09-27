/*******************************************************************
*
*  DESCRIPTION: String utility functions
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __STRINGP_H
#define __STRINGP_H
#include <string>
#include <cstdio>

template< class T >
inline
std::string opadd( const char *format, T val )
{
	static char buf[10];
	sprintf( buf, format, val );
	return std::string( buf );
}

inline
std::string operator +( const std::string &str, double val )
{
	return str + opadd( "%03.3g", val );
}

inline
std::string operator +( const std::string &str, int val )
{
	return str + opadd( "%02d", val );
}

inline
std::string operator +( const std::string &str, unsigned int val )
{
	return str + opadd( "%02d", val );
}

#endif   //__STRINGP_H 
