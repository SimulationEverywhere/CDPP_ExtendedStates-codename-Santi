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

#ifndef __LOG_H
#define __LOG_H

#include <string>

/** declarations **/
class Log
{
public:
	enum LogType {
		logAll = 255 ,
		logCollect = 1,
		logInternal = 2,
		logDone = 4,
		logOutput = 8,
		logExternal = 16,
		logInit = 32,
		logSent = 64,
		logNone = 0
	};


	Log();

	Log& createLog( bool );
	bool createLog();

	Log& logToStdOut( bool );
	bool logToStdOut();

	std::string filename();
	Log& filename( const char *);
	Log& filename( const std::string& );

	Log& logType( const char* );		//creates the logtype from the command line modifiers.
	unsigned logType();

	static Log Default;

private:
	bool blLogToStdOut;
	std::string strLogFileName;
	unsigned type;

}; // Log

/** inlines **/

inline Log::Log() : 
blLogToStdOut(true)
	, type(logNone)
{}


inline bool Log::createLog() 
{
	return (type != logNone);
}


inline bool Log::logToStdOut() 
{
	return blLogToStdOut;
}

inline std::string Log::filename()
{
	return strLogFileName;
}



inline unsigned Log::logType()
{
	return type;
}

#endif //__LOG_H
