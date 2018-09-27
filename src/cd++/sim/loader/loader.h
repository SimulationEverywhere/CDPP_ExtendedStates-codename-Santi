/*******************************************************************
*
*  DESCRIPTION: CLASS SimLoader
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 11/04/1999 (v2)
*  DATE: 29/08/2000 (v3)
*
*******************************************************************/

#ifndef __LOADER_H
#define __LOADER_H

/** include files **/
#include "VTime.h"
#include "evaldeb.h"		// EvalDebug

/** forward declarations **/
class iostream  ;
class ParallelMainSimulator ;

/** declarations **/

class SimLoader
{
public:
	virtual SimLoader &openComm() = 0;
	virtual SimLoader &closeComm() = 0;

	virtual std::istream &modelsStream()
	{return *models;}

	virtual std::istream &eventsStream()
	{return *events;}

	virtual std::istream &partitionStream()
	{return *partition;}

	virtual std::string logName()
	{return log;}

	virtual std::string outputName()
	{return output;}

	virtual std::string partitionDebugName(){
		return partitionDebug;	
	}

	virtual bool logSentMessages() 
	{	return logSend;}

	const VTime &stopTime() const
	{return stop;}

	int getMachineID() const
	{return machineID;}

	bool printParserInfo() const
	{return printParser;}

	virtual SimLoader &loadData() = 0;     		

	//MPI requires the command line arguments to initialize himself
	SimLoader( int pargc, char** pargv)
	: models( NULL )
		, events( NULL )
		, partition ( NULL )
		, argc( pargc )
		, argv( pargv )
		, log("/dev/null" )
		, output("/dev/null")
		, partitionDebug("/dev/null")
		, logSend(false)
	{	EvalDebug().Stream( NULL );
		EvalDebug().Active( false );
	}


	virtual ~SimLoader(){}				//Destructor

protected:
	friend class ParallelMainSimulator;

	// ** Instance variables ** //
	VTime stop ;
	bool printParser ;
	std::istream *models ;
	std::istream *events ;
	std::istream *partition;
	int machineID;
	int argc;
	char** argv;	
	std::string log ;
	std::string output;
	std::string partitionDebug;
	bool logSend;


	// ** Methods ** //
	SimLoader()					//Default constructor
		: models( NULL )
		, events( NULL )
		, partition ( NULL )
		, log("/dev/null" )
		, output("/dev/null")
		, partitionDebug("/dev/null")
		, logSend(false)
	{	EvalDebug().Stream( NULL );
		EvalDebug().Active( false );
	}



	virtual SimLoader &writeResults() = 0;

	SimLoader &stopTime( const VTime &t )
	{stop = t; return *this;}

	SimLoader &printParserInfo (const bool pp)
	{printParser = pp; return *this;}



};	// class SimLoader

#endif   //__LOADER_H 
