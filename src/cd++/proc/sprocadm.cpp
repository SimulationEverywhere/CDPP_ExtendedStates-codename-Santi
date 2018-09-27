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

/** include files **/
#include "sprocadm.h"          // header
#include "simulat.h"          // class Simulator
#include "coordin.h"          // class Coordinator
#include "strutil.h"          // lowerCase
#include "root.h"             // class Root
#include "coorcell.h"         // CellCoordinator
#include "flatcoor.h"         // FlatCoordinator
#include "flatcoup.h"         // FlatCoupledCell

using namespace std;

/** public data **/
const ProcId StandAloneProcessorAdmin::rootId( 0 ) ;

/** private data **/
StandAloneProcessorAdmin *SingleStandAloneProcessorAdmin::instance( NULL ) ;


/** public functions **/
/*******************************************************************
* Function Name: ProcessorAdmin
********************************************************************/
StandAloneProcessorAdmin::StandAloneProcessorAdmin() 
	: procIdCount( StandAloneProcessorAdmin::rootId + 1 )
{}

/*******************************************************************
* Function Name: ~ProcessorAdmin()
* Description: destroys all the alive processors
********************************************************************/
StandAloneProcessorAdmin::~StandAloneProcessorAdmin()
{
	for ( ProcessorDB::iterator cursor = procDB.begin(); cursor != procDB.end(); cursor++ )
		delete cursor->second;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
Processor &StandAloneProcessorAdmin::generateProcessor( Atomic *atomic )
{
	Simulator *sim = new Simulator( atomic );
	add2DB( sim, newId() );
	return *sim;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
Processor &StandAloneProcessorAdmin::generateProcessor( Coupled *coupled )
{
	Coordinator *coord = new Coordinator( coupled );
	add2DB( coord, newId() );
	return *coord;
}

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
Processor &StandAloneProcessorAdmin::generateProcessor( CoupledCell *coupled )
{
	Coordinator *coord = new CellCoordinator( coupled ) ;
	add2DB( coord, newId() ) ;
	return *coord ;
} 

/*******************************************************************
* Function Name: generateProcessor
********************************************************************/
Processor &StandAloneProcessorAdmin::generateProcessor( FlatCoupledCell *coupled )
{
	Coordinator *coord = new FlatCoordinator( coupled ) ;
	add2DB( coord, newId() ) ;
	return *coord ;
} 

/*******************************************************************
* Function Name: generateRoot
********************************************************************/
Processor &StandAloneProcessorAdmin::generateRoot()
{
	Root *root = new Root( StandAloneProcessorAdmin::rootId ) ;
	add2DB( root, StandAloneProcessorAdmin::rootId ) ;
	return *root ;
}

/*******************************************************************
* Function Name: processor
********************************************************************/
Processor &StandAloneProcessorAdmin::processor( const ProcId &id )
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
Processor &StandAloneProcessorAdmin::processor( const string &name )
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
StandAloneProcessorAdmin &StandAloneProcessorAdmin::add2DB( Processor *newProc, const ProcId &newId )
{
	newProc->id( newId );
	procDB[ newId ] = newProc;
	return *this;
}
