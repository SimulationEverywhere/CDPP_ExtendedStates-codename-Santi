/*******************************************************************
*
*  DESCRIPTION: class ParallelProcessorAdmin
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 29/9/2000
*
*******************************************************************/

#ifndef __PPROCESSOR_ADMIN_H
#define __PPROCESSOR_ADMIN_H

/** include files **/
#include <map>
#include <iostream>
#include "pprocess.h"          // ProcId

/** forward declarations **/
class Model;
class RootModel;
class Atomic;
class Coupled;
class CoupledCell ;
class FlatCoupledCell;
class SingleParallelProcessorAdmin ;
class ParallelRoot ;
class ParallelProcessor ;

/** declarations **/

class ParallelProcessorAdmin 
{
public:
	~ParallelProcessorAdmin() ; // Destructor

	ParallelProcessor &generateProcessor( Atomic * , const ProcId);
	ParallelProcessor &generateProcessor( Coupled *, const ProcId);
	ParallelProcessor &generateProcessor( CoupledCell *, const ProcId);
	ParallelProcessor &generateProcessor( FlatCoupledCell *, const ProcId);
	ParallelProcessor &generateRoot( RootModel *, const ProcId);

	ParallelProcessor &processor( const ProcId & ) ;
	ParallelProcessor &processor( const std::string & ) ;

	void   model( const ProcId &, Model *);
	Model &model( const ProcId &);

	void showProcessors( std::ostream &out = std::cout);

	static const ProcId rootId ;

	const ProcId newId()
	{return procIdCount++;}

private:
	friend class SingleParallelProcessorAdmin ;
	friend class ParallelRoot ;

	typedef std::map< ProcId, ParallelProcessor *, std::less< ProcId > > ProcessorDB ; 
	typedef std::map< ProcId, Model *, std::less <ProcId> > ProcessorModelDB;

	ProcessorDB procDB ;
	ProcessorModelDB procModelDB;
	ProcId procIdCount ;

	ParallelProcessorAdmin();	// Default constructor

	ParallelProcessorAdmin &add2DB( ParallelProcessor *newProc, const ProcId &newId ) ;

};	// class ParallelProcessorAdmin


class SingleParallelProcessorAdmin
{

public:
	static ParallelProcessorAdmin &Instance() ;

private:
	static ParallelProcessorAdmin *instance ;

} ;

/** inline **/
inline
	ParallelProcessorAdmin &SingleParallelProcessorAdmin::Instance()
{
	if( !instance ) {
		instance = new ParallelProcessorAdmin();        	
	}

	return *instance;

}

#endif   //__PPROCESSOR_ADMIN_H 
