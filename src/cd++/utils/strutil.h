/*******************************************************************
*
*  DESCRIPTION: String utility functions
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 18/02/1999 (v2)
*
*******************************************************************/

#ifndef __STRUTIL_H
#define __STRUTIL_H

#include <ctype.h>   // tolower 
#include <cstdio>
// If OS is Apple based, hashmap is defined differently
#if defined __GNUC__ || defined __APPLE__
       #include <ext/hash_map>
#else
       #include <hash_map>
#endif
#include "real.h"

std::string lowerCase( const std::string &str );
float str2float( const std::string &str );
Value str2Value( const std::string &str );

inline std::string Value2StrReal( const double f)
{ Real r(f); return r.asString(); }

inline float char2float( const char c )
{return c - '0';}

inline Real str2Real( const std::string &str )
{ return str[0] == '?' ? Real() : Real( str2Value(str)); }

int str2Int( const std::string &str );
std::string int2Str( int val );
bool isNumeric( const std::string &str);
bool isDigitOrSpace( const std::string &str );
std::string trimSpaces( const std::string &str );
std::string trimSpaces( const char *str );
std::string substringLenght( const std::string str, int len );
std::string upcase( const std::string str );

//using namespace __gnu_cxx;

class HashString
{
public:
	size_t operator()(const std::string &str) const
	{
		__gnu_cxx::hash<const char*> h;
		return (h(str.c_str()));
	}
};


#endif   //__STRUTIL_H
