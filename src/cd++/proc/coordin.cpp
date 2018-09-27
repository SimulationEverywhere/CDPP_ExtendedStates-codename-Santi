/*******************************************************************
*
*  DESCRIPTION: class Coordinator
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  VERSION 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE:  4/6/1999 (v2)
*
*******************************************************************/

/** include files **/
#include "coordin.h"       // header
#include "msgadm.h"        // SingleMsgAdmin 
#include "coupled.h"       // class Coupled
#include "procadm.h"

using namespace std;

/*******************************************************************
* Function Name: Coordinator
********************************************************************/
Coordinator::Coordinator( Coupled *coupled )
	: Processor( coupled )
	, doneCount( 0 )
	, receivedX( 1 )
	, inminent( Processor::InvalidId )
{}

/*******************************************************************
* Function Name: receive
* Description: forward the init message to his childrens
********************************************************************/
Processor &Coordinator::receive( const InitMessage &msg )
{
	Coupled &coupled( static_cast<Coupled &>( model() ) );
	
	doneCount = coupled.children().size();
	
	Coupled::ModelList::const_iterator cursor;
	
	InitMessage init( msg.time(), this->Processor::id() );
	
	for( cursor = coupled.children().begin() ; cursor != coupled.children().end() ; cursor++ )
		SingleMsgAdm::Instance().send( init , *cursor );
	
	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: sends a X message to the port's influences
********************************************************************/
Processor &Coordinator::receive( const ExternalMessage &msg )
{
	const InfluenceList &influList( msg.port().influences() ) ;
	InfluenceList::const_iterator cursor( influList.begin() ) ;
	
	// Old Code //////////////////////////////////////////////////////////
	//MASSERT( !doneCount );
	//doneCount = influList.size() ;	// No permite eventos externos consecutivos
	//////////////////////////////////////////////////////////////////////
	
	// New Code //////////////////////////////////////////////////////////
	if (doneCount == 0)
		receivedX = 1;
	else
		receivedX++;
	
	doneCount += influList.size() ;	
	//////////////////////////////////////////////////////////////////////
	
	if( doneCount > 0 )
	{
		ExternalMessage xMsg( msg ) ;
		xMsg.procId( this->Processor::id() ) ;
		
		for( ; cursor != influList.end(); cursor++ )
		{
			xMsg.port( *(*cursor) ) ;
			SingleMsgAdm::Instance().send( xMsg, (*cursor)->model() ) ;
		}
	} else
	{
		lastChange( msg.time() ) ;
		
		if( inminentChild() != Processor::InvalidId )
		{
			nextChange( model().absoluteNext() - msg.time() ) ;
			
			Processor &proc( SingleProcessorAdmin::Instance().processor( inminentChild() ) ) ;
			nextChange( proc.model().absoluteNext() - msg.time() ) ;
		}
		DoneMessage doneMsg( msg.time(), this->Processor::id(), nextChange() ) ;
		SingleMsgAdm::Instance().send( doneMsg, model().parentId() ) ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: sends the * message to the inminent child
********************************************************************/
Processor &Coordinator::receive( const InternalMessage &msg )
{
	doneCount = 1 ;
	
	InternalMessage internal( msg.time(), this->Processor::id() ) ;
	
	SingleMsgAdm::Instance().send( internal, inminentChild() ) ;
	
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: translates the output event to a X messages for the 
*              influenced children and to a Y message for his father
********************************************************************/
Processor &Coordinator::receive( const OutputMessage &msg )
{
	Coupled &coupled( static_cast< Coupled & >( model() ) );
	
	const InfluenceList &influList( msg.port().influences() );
	
	InfluenceList::const_iterator cursor( influList.begin() );
	
	OutputMessage outMsg( msg );
	outMsg.procId( this->Processor::id() );
	
	ExternalMessage extMsg;
	extMsg.time( msg.time() );
	extMsg.procId( Processor::id() );
	extMsg.value( msg.value() );
	
	for( ; cursor != influList.end(); cursor++ )
	{
		// not found in the output list => send it as an external message
		if( coupled.outputPorts().find( (*cursor)->id() ) == coupled.outputPorts().end() )
		{
			doneCount++;
			extMsg.port( * (*cursor) );
			SingleMsgAdm::Instance().send( extMsg, (*cursor)->model() );
		} else
		{
			outMsg.port( * (*cursor) );
			SingleMsgAdm::Instance().send( outMsg, coupled.parentId() );
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: recalculates the inminent child and sends it to his 
*              father inside a done message
********************************************************************/
Processor &Coordinator::receive( const DoneMessage &msg )
{
	Coupled &coupled( static_cast< Coupled & >( model() ) );
	
	MASSERTMSG( doneCount > 0, "Unexpected Done message!" );
	
	if( ! --doneCount )
	{
		lastChange( msg.time() );
		
		recalcInminentChild();
		
		if( inminentChild() == Processor::InvalidId )
			nextChange( VTime::Inf );
		else
		{
			Processor &proc = SingleProcessorAdmin::Instance().processor( inminentChild() );
			nextChange( proc.model().absoluteNext() - msg.time() );
		}
		DoneMessage doneMsg( msg.time(), this->Processor::id(), coupled.nextChange() );
		
		for (long i = 1; i <= receivedX; i++)
			SingleMsgAdm::Instance().send( doneMsg, coupled.parentId() );
		
		receivedX = 1;
	}
	return *this;
}

/*******************************************************************
* Function Name: id
********************************************************************/
Processor &Coordinator::id( const ProcId &pid )
{
	this->Processor::id( pid );
	
	MASSERT( pmodel() );
	pmodel()->id( pid );
	
	return *this;
}

/*******************************************************************
* Function Name: recalcInminentChild
********************************************************************/
Coordinator &Coordinator::recalcInminentChild()
{
	Coupled &coupled( static_cast< Coupled & >( model() ) );
	
	Coupled::ModelList::const_iterator cursor( coupled.children().begin() ) ;
	
	VTime min( VTime::Inf );
	inminentChild( Processor::InvalidId );
	
	for( ; cursor != coupled.children().end() ; cursor++ )
	{
		Model &model( SingleProcessorAdmin::Instance().processor( *(cursor) ).model() ) ;
		
		if( min > model.absoluteNext() )
		{
			min = model.absoluteNext();
			inminentChild( *(cursor) );
		}
	}
	return *this;
}
