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
*  DATE: 12/06/1999 (v2)
*
*******************************************************************/

#include "strutil.h"

using namespace std;

float str2float( const string &str )
{
	register float v ;
	sscanf( str.c_str(), "%f", &v ) ;
	return v ;
}

Value str2Value( const string &str )
{
	register float v ;
	sscanf( str.c_str(), "%80g", &v ) ;
	return v ;
}

int str2Int( const string &str )
{
	register int v ;
	sscanf( str.c_str(), "%d", &v );
	return v ;
}

string int2Str( int val )
{
	static char s[15];

	sprintf(s, "%d", val);
	return string(s);
}

bool isNumeric( const string &str) {
	string::const_iterator curs;

	for( curs = str.begin() ;
		curs != str.end() && ( *curs >= '0' && *curs <= '9' ) ;
		curs ++ ) ;

	return curs == str.end() ;
}

bool isDigitOrSpace( const string &str )
{
	string::const_iterator curs ;

	for( curs = str.begin() ; 
		curs != str.end() && ( *curs == ' ' || *curs == '\t' || ( *curs >= '0' && *curs <= '9' ) ) ;
		curs ++ ) ;

	return curs == str.end() ;
}

string trimSpaces( const string &str )
{
	string s("");

	for ( string::const_iterator curs = str.begin(); curs != str.end(); curs ++ )
		if ( *curs != ' ' )
			s += *curs;

	return s;
}

string trimSpaces( const char *str )
{
	string	s("");

	while (*str != 0)
	{
		if (*str != ' ')
			s += *str;
		str++;
	}
	return s;
}

string substringLenght( const string str, int len )
{
	string	sAux("");
	int l = 0;

	while (l < len)
		sAux += str[l++];

	return sAux;
}

string upcase( const string str )
{
        string  sAux("");
	register const char	*sc = str.c_str();

	while (*sc)
	{
		sAux += toupper(*sc);
		sc++;
	}
        return sAux;
}

string lowerCase( const string &str )
{
        string  sAux("");
	register const char	*sc = str.c_str();

	while (*sc)
	{
		sAux += tolower(*sc);
		sc++;
	}
        return sAux;
}
