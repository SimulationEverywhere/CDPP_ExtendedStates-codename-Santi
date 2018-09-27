/*******************************************************************
*
*  DESCRIPTION: Utilities to parse the simulation log
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 19/04/2001
*
*******************************************************************/

#include "logparser.h"

using namespace std;

/********************************************************************
* parseLine:
* Returns True if line pointed by l contains an output message from a cell in modelName.
* VTime current will be filled with the message time, posi with the CellPosition and value with the output value
********************************************************************/

bool parseLine( const char *l, VTime &current, const string &modelName, CellPosition &posi, Real &value, const string &portName )
{
	istrstream line( l ) ;

	istream_iterator<string> cursor( line ) ;
	string tokens[ 15 ];
	register long pos( 0 ) ;

	while( cursor != istream_iterator<string>() && pos < 15 )
		tokens[ pos++ ] = *cursor++ ;

	if( pos == 15 && tokens[2] == "L" && tokens[4] == "Y" && \
	    tokens[8].find( modelName ) == 0 && tokens[14].find( modelName ) == 0 && \
	    (tokens[10].empty() || tokens[10] == portName))
	{
		// the time of the message
		current = tokens[ 6 ] ; 

		CellPosition pos( tokens[8] ) ;
		posi = pos ;
		value = Real( str2Real(tokens[ 12 ]) ) ;
		return true ;
	}
	return false ;
}

/********************************************************************
* isMessageLine:
* Returns True if line pointed by l contains a valid message
* msgTime wil lbe filled with the time of the message, and LP with the number of the LP that receives the message
********************************************************************/

bool isMessageLine( const char* l, int& LP, VTime &msgTime)
{
	istrstream line( l ) ;

	istream_iterator<string> cursor( line ) ;
	string tokens[ 15 ];
	register long pos( 0 ) ;

	while( cursor != istream_iterator<string>() && pos < 15 )
		tokens[ pos++ ] = *cursor++ ;

	if( (tokens[4] == "*" && pos == 11 ) || (tokens[4] == "@" && pos == 11) || (tokens[4] == "$" && pos == 11)
			|| (tokens[4] == "Y" && pos == 15 ) || (tokens[4] == "X"  && pos == 15)
			|| (tokens[4] == "I" && pos == 11) ||( tokens[4] == "D" && pos == 13))
	{
		// the time of the message
		msgTime= tokens[ 6 ] ; 
		LP = str2Int( tokens[0]);
		return true ;
	}
	return false ;
}

/********************************************************************
* isMessageLine:
* Returns True if line pointed by l contains a valid message
* msgTime will be filled with the time of the message, LP with the number of the LP that receives the message
* msgType gets the message type
********************************************************************/

bool isMessageLine( const char* l, int& LP, VTime &msgTime, string& msgType)
{
	istrstream line( l ) ;

	istream_iterator<string> cursor( line ) ;
	string tokens[ 15 ];
	register long pos( 0 ) ;

	while( cursor != istream_iterator<string>() && pos < 15 )
		tokens[ pos++ ] = *cursor++ ;

	msgType = tokens[4] ;

	if( (tokens[4] == "*" && pos == 11 ) || (tokens[4] == "@" && pos == 11) || (tokens[4] == "$" && pos == 11)
			|| (tokens[4] == "Y" && pos == 15 ) || (tokens[4] == "X"  && pos == 15)
			|| (tokens[4] == "I" && pos == 11) ||( tokens[4] == "D" && pos == 13))
	{
		// the time of the message
		msgTime= tokens[ 6 ] ; 
		LP = str2Int( tokens[0]);
		return true ;
	}
	return false ;
}
