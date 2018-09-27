/*******************************************************************
*
*  DESCRIPTION: class ProcessorAdmin, SingleProcessorAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __PROCESSOR_ADMIN_H
#define __PROCESSOR_ADMIN_H

/** include files **/
#include <map>
#include "process.h"          // ProcId

/** forward declarations **/
class Atomic;
class Coupled;
class CoupledCell ;
class FlatCoupledCell;
class SingleProcessorAdmin ;
class Root ;

/** declarations **/

class ProcessorAdmin
{
public:
	virtual ~ProcessorAdmin() ; // Destructor

	virtual Processor &generateProcessor( Atomic * ) = 0;
	virtual Processor &generateProcessor( Coupled * ) = 0;
	virtual Processor &generateProcessor( CoupledCell * ) = 0;
	virtual Processor &generateProcessor( FlatCoupledCell * ) = 0;
	virtual Processor &generateRoot() = 0;

	virtual Processor &processor( const ProcId & ) = 0 ;
	virtual Processor &processor( const std::string & ) = 0;

	static const ProcId rootId ;

};	// class ProcessorAdmin


// typedef Singleton< ProcessorAdmin > SingleProcessorAdmin ;

class SingleProcessorAdmin
{

public:
	static ProcessorAdmin &Instance() ;
	static ProcessorAdmin &CreateStandAlone();
	static ProcessorAdmin &CreateParallel();

private:
	static ProcessorAdmin *instance ;

} ;

/** inline **/
inline
ProcessorAdmin &SingleProcessorAdmin::Instance()
{
	MASSERTMSG( instance, "No SingleProcessorAdm (Parallel or StandAlone) has been defined." );
	return *instance;

}

#endif   //__PROCESSOR_ADMIN_H 
