/*******************************************************************
*
*  DESCRIPTION: class CellCoordinator
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 27/2/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include "coorcell.h"      // class header
#include "coupcell.h"      // class CupledCell
#include "msgadm.h"        // SingleMsgAdmin

using namespace std;

// ** public data ** //

// ** private data ** //             

// ** public ** //

/*******************************************************************
* Function Name: CellCoordinator
* Description:
********************************************************************/
CellCoordinator::CellCoordinator( CoupledCell *cell )
	: Coordinator( cell )
{
	receivedX = 1;
}


/*******************************************************************
* Function Name: receive
* Description: 
********************************************************************/
Processor &CellCoordinator::receive( const InternalMessage &msg )
{
	doneCount = 0 ;
	InternalMessage internal( msg.time(), this->Processor::id() ) ;
	
	Inminents::iterator cursor = inminents.find( msg.time() );
	for( ; cursor != inminents.end() && cursor->first == msg.time() ; cursor++ , this->doneCount++ )
		SingleMsgAdm::Instance().send( internal, cursor->second ) ;
	
	inminents.erase( inminents.find( msg.time() ), cursor );
	
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: 
********************************************************************/
Processor &CellCoordinator::receive( const OutputMessage &msg )
{
	Coupled &coupled( static_cast< Coupled & >( model() ) );
	
	const InfluenceList &influList( msg.port().influences() );
	
	InfluenceList::const_iterator cursor( influList.begin() );
	
	OutputMessage outMsg( msg ) ;
	outMsg.procId( this->Processor::id() ) ;
	
	ExternalMessage extMsg ;
	extMsg.time( msg.time() ) ;
	extMsg.procId( this->Processor::id() ) ;
	extMsg.value( *msg.value() ) ;
	
	for( ; cursor != influList.end() ; cursor++ )
	{
		// missing in the output list => must be treated as an external message
		if( coupled.outputPorts().find( (*cursor)->id() ) == coupled.outputPorts().end() )
		{
			extMsg.port( * (*cursor) ) ;
			
			// send the message only once per model
			if( influenced.find( (*cursor)->modelId() ) == influenced.end() )
			{
				SingleMsgAdm::Instance().send( extMsg, (*cursor)->modelId() ) ;
				doneCount++ ;
			}
			
			influenced.insert( (*cursor)->modelId() ) ;
		} else
		{
			outMsg.port( * (*cursor) ) ;
			SingleMsgAdm::Instance().send( outMsg, coupled.parentId() ) ;
		}
	}
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: 
********************************************************************/
Processor &CellCoordinator::receive( const DoneMessage &msg )
{
	Coupled &coupled( static_cast< Coupled & >( this->model() ) ) ;
	
	MASSERTMSG( this->doneCount > 0, "Unexpected Done message!" ) ;
	
	Inminents::iterator cursor ;
	for( cursor = inminents.begin() ; cursor != inminents.end() && (*cursor).second != msg.procId() ; cursor++ ) ;
	if( cursor != inminents.end() )
		inminents.erase( cursor );
	
	if( msg.nextChange() != VTime::Inf )
	{
		VTime t( msg.time() + msg.nextChange() );
		
		Inminents::iterator cursor ;
		for( cursor = inminents.find( t ) ; 
		cursor != inminents.end() && cursor->first == t && cursor->second > msg.procId() ; 
		cursor ++ )	;
		
		// the inminents list it's ordered by (time, procid)
		inminents.insert( cursor, Inminents::value_type( t, msg.procId() ) ) ;
	}
	
	doneCount-- ;
	
	// if donecount == 0 then all the children already answered to they messages
	if( doneCount == 0 )
	{
		lastChange( msg.time() );
		
		if( inminents.empty() )
		{
			nextChange( VTime::Inf );
			inminentChild( Processor::InvalidId );
		} else
		{
			cursor = inminents.begin();
			
			// nextchange relative
			nextChange( cursor->first - msg.time() );
			inminentChild( cursor->second );
		}
		
		// the set must be erased 
		influenced.erase( influenced.begin(), influenced.end() );
		
		DoneMessage doneMsg( msg.time(), this->Processor::id(), coupled.nextChange(), false ); 
		
		for (long i = 1; i <= receivedX; i++)
			SingleMsgAdm::Instance().send( doneMsg, coupled.parentId() );
		
		receivedX = 1;
	}
	return *this ;
}
