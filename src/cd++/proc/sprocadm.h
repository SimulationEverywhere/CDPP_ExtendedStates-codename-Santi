/*******************************************************************
*
*  DESCRIPTION: class StandAloneProcessorAdmin
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 29/9/2000
*
*******************************************************************/

#ifndef __SPROCESSOR_ADMIN_H
#define __SPROCESSOR_ADMIN_H

/** include files **/
#include <map>
#include "sprocadm.h"          // 
#include "process.h"          // ProcId

/** forward declarations **/
class Atomic;
class Coupled;
class CoupledCell ;
class FlatCoupledCell;
class SingleStandAloneProcessorAdmin ;
class Root ;

/** declarations **/

class StandAloneProcessorAdmin 
{
public:
	
	virtual ~StandAloneProcessorAdmin() ; // Destructor
	
	virtual Processor &generateProcessor( Atomic * ) ;
	virtual Processor &generateProcessor( Coupled * ) ;
	virtual Processor &generateProcessor( CoupledCell * ) ;
	virtual Processor &generateProcessor( FlatCoupledCell * ) ;
	virtual Processor &generateRoot() ;
	
	virtual Processor &processor( const ProcId & ) ;
	virtual Processor &processor( const std::string & ) ;
	
	static const ProcId rootId ;
	
private:
	typedef std::map< ProcId, Processor *, std::less< ProcId > > ProcessorDB ; 
	
	ProcessorDB procDB ;
	ProcId procIdCount ;
	
	friend class SingleStandAloneProcessorAdmin ;
	friend class Root ;
	
	static StandAloneProcessorAdmin *instance;
	
	StandAloneProcessorAdmin();	// Default constructor
	StandAloneProcessorAdmin &add2DB( Processor *newProc, const ProcId &newId ) ;
	
	const ProcId newId()
	{return procIdCount++;}
	
};	// class StandAloneProcessorAdmin



class SingleStandAloneProcessorAdmin
{
	
public:
	static StandAloneProcessorAdmin &Instance() ;
	
private:
	static StandAloneProcessorAdmin *instance ;
	
} ;

/** inline **/
inline
	StandAloneProcessorAdmin &SingleStandAloneProcessorAdmin::Instance()
{
	if( !instance ) {
		instance = new StandAloneProcessorAdmin();        	
	}
	
	return *instance;
}

#endif   //__SPROCESSOR_ADMIN_H 
