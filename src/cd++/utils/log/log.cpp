/*******************************************************************
*
*  DESCRIPTION: class Log
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 02/04/2001
*
*******************************************************************/

#include "log.h"
#include <iostream>

using namespace std;

/*Single Instance of the Log object*/

Log Log::Default;

Log& Log::createLog( bool log ) 
{

	if (log && type == logNone)
		type = logAll ^ logSent;
	else if ( !log )
		type = logNone;

	return *this;	
}

Log& Log::logToStdOut( bool logToStdOut )
{
	blLogToStdOut = logToStdOut;
	return *this;
}

Log& Log::filename( const string& name)
{
	strLogFileName = name;

	if( strLogFileName == "" )
		logToStdOut( true );
	else
		logToStdOut( false );

	return *this;	
}

Log& Log::filename( const char * name)
{
	strLogFileName = string(name);

	if( strLogFileName == "" )
		logToStdOut( true );
	else
		logToStdOut( false );

	return *this;
}

Log& Log::logType( const char* modif )
{
	unsigned i;

	type = logNone;

	for(i = 0; modif[i] != '\0'; i++)
	{
		switch (modif[i])
		{
			case '*':
				type = type | logInternal;
				break;

			case '@':
				type = type | logCollect;
				break;

			case 'D':
				type = type | logDone;
				break;

			case 'I':
				type = type | logInit;
				break;

			case 'Y':
				type = type | logOutput;
				break;

			case 'X':
				type = type | logExternal;
				break;

			case 'S':
				type = type | logSent;
				break;

			default:
				cerr << "Warning: Invalid log modifier " << modif[i] << "." << endl;
		}
	}

	return *this;
}
