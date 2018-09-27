/*******************************************************************
*
*  DESCRIPTION: class ParallelProcessorAdmin
*
*  AUTHOR: Alejandro Troccoli
*
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 04/10/2000
*******************************************************************/

/** include files **/
#include "pprocadm.h"          // header
#include "model.h"		   // class Model
#include "atomic.h"
#include "flatcoup.h"	
#include "coupled.h"
#include "psimulat.h"          // class Simulator
#include "pcoordin.h"          // class Coordinator

#include "pmcoordin.h"	   // class ParallelMCoordinator
#include "pscoordin.h"	   // class ParallelSCoordinator

#include "strutil.h"           // lowerCase
#include "proot.h"             // class Root

using namespace std;

/** public data **/
const ProcId ParallelProcessorAdmin::rootId( 0 ) ;

/** private data **/
ParallelProcessorAdmin *SingleParallelProcessorAdmin::instance( NULL ) ;

/** public functions **/
/*******************************************************************
* Function Name: ProcessorAdmin
********************************************************************/
ParallelProcessorAdmin::ParallelProcessorAdmin()
{	
	procIdCount = ParallelProcessorAdmin::rootId + 1;
}

/*******************************************************************
* Function Name: ~ProcessorAdmin()
* Description: destroys all the alive processors
********************************************************************/
ParallelProcessorAdmin::~ParallelProcessorAdmin()
{
	for ( ProcessorDB::iterator cursor = procDB.begin(); cursor != procDB.end(); cursor++ )
		delete cursor->second;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::generateProcessor( Atomic *atomic, const ProcId id )
{
	ParallelSimulator *sim = new ParallelSimulator( atomic );
	atomic->processor( (ParallelProcessor *)sim);
	add2DB( sim, id );
	return *sim;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::generateProcessor( Coupled *coupled, const ProcId id )
{

	ParallelCoordinator *coord;
#ifndef NEWCOORDIN
	//Coordinator can be either master or slave.
	if (coupled->isLocalMaster())
		coord = new ParallelMCoordinator( coupled );
	else
		coord = new ParallelSCoordinator( coupled );
#else
	//Coordinator can be either master or slave.
	if (coupled->isLocalMaster())
		coord = new ParallelMCoordinator2( coupled );
	else
		coord = new ParallelSCoordinator2( coupled );

#endif
	coupled->processor( (ParallelProcessor *)coord );
	add2DB( coord, id );
	return *coord;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::generateProcessor( CoupledCell *coupled, const ProcId id )
{

	return generateProcessor( (Coupled*) coupled, id );

} 

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::generateProcessor( FlatCoupledCell *coupled, const ProcId id )
{
	ParallelCoordinator *coord;// = new ParallelCoordinator( coupled ) ;
	coupled->processor( (ParallelProcessor*) coord );
	add2DB( coord, id ) ;
	return *coord ;
} 

/*******************************************************************
* Function Name: generateRoot
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::generateRoot( RootModel * rootmdl, const ProcId pid)
{
	ParallelRoot *root = new ParallelRoot( rootmdl ) ;
	rootmdl->processor( (ParallelProcessor*) root );
	add2DB( root, pid ) ;
	return *root ;
}

/*******************************************************************
* Function Name: processor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::processor( const ProcId &id )
{
	ProcessorDB::iterator cursor( procDB.find( id ) );

	if( cursor == procDB.end() )
	{
		InvalidProcessorIdException e;
		e.addText( string( "The processor id " ) + id + " is invalid!" ) ;
		MTHROW( e ) ;
	}
	MASSERT( cursor->second );
	return *( cursor->second );
}

/*******************************************************************
* Function Name: processor
********************************************************************/
ParallelProcessor &ParallelProcessorAdmin::processor( const string &name )
{
	string lower( lowerCase( name ) ) ;

	ProcessorDB::iterator cursor( procDB.begin() );

	while( cursor != procDB.end() && cursor->second->description() != lower )
		cursor ++ ;

	if( cursor == procDB.end() )
	{
		InvalidProcessorIdException e ;
		e.addText( string( "The processor " ) + lowerCase( name ) + " is invalid!" );
		MTHROW( e ) ;
	}
	return *( cursor->second );
}

/*******************************************************************
* Function Name: add2DB
********************************************************************/
ParallelProcessorAdmin &ParallelProcessorAdmin::add2DB( ParallelProcessor *newProc, const ProcId &newId )
{
	newProc->id( newId );
	procDB[ newId ] = newProc;
	return *this;
}



/*******************************************************************
* Function Name: model
********************************************************************/
void   ParallelProcessorAdmin::model( const ProcId & pid, Model * model ) {

	procModelDB[pid] = model;
}

/*******************************************************************
* Function Name: model
********************************************************************/

Model &ParallelProcessorAdmin::model( const ProcId & pid) {

	ProcessorModelDB::iterator cursor( procModelDB.find( pid ) );

	if( cursor == procModelDB.end() )
	{
		InvalidProcessorIdException e;
		e.addText( string( "The processor id " ) + pid + " is invalid!" ) ;
		MTHROW( e ) ;
	}
	MASSERT( cursor->second );
	return *( cursor->second );
}

/*******************************************************************
* Function Name: showProcessors
********************************************************************/
void ParallelProcessorAdmin::showProcessors( ostream &out)
{
	ProcessorDB::iterator cursor( procDB.begin() );

	out << "Current processors: " << endl;

	while( cursor != procDB.end()) {
		ParallelProcessor *proc = cursor->second;
		out << "Processor Id: " << proc->id() << "\tDescription: " << proc->description() << endl;
		out << "\tModel Id: " << (proc->model()).id() << " " << proc->model().asString() << endl;
		out << "\tParent Id: " << proc->model().parentId()<< endl;
		cursor ++;
	}

}
