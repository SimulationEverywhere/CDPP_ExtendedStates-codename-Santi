/*******************************************************************
*
*  DESCRIPTION: class ParallelCoordinator
*
*  AUTHOR:	Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE:  16/11/2000
*
*******************************************************************/

/** include files **/
#include "pcoordin.h"       // header
#include "coupled.h"       // class Coupled
#include "pprocadm.h"	
#include "pmessage.h"
#include "parsimu.h"		//class ParallelMainSimulator

using namespace std;

/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelCoordinator::ParallelCoordinator( Coupled * coupled ) 
	: ParallelProcessor(coupled)
{

	this->id(coupled->localProc());

}

/*******************************************************************
* Function Name: rollbackCheck
********************************************************************/
bool ParallelCoordinator::rollbackCheck(const VTime& currentTime) 
{
	bool rollback = ParallelProcessor::rollbackCheck( currentTime );

	if ( rollback ) 
	{
		doneCount ( 0 );
		synchronizeList.erase( synchronizeList.begin(), synchronizeList.end() );
	}

	return rollback;
}

/*******************************************************************
* Function Name: initialize
********************************************************************/
void ParallelCoordinator::initialize() 
{
	ParallelProcessor::initialize();

	doneCount( 0 );

	//Add all the local master processor for the child models as 
	//dependents.

	Coupled &coupled = static_cast< Coupled &>( model());
	Coupled::PModelList::const_iterator cursor;

	for( cursor = coupled.childModels().begin(); cursor != coupled.childModels().end();
	cursor++){
		if ( (*cursor)->isLocalMaster()) {
			dependants()[(*cursor)->localProc()] = VTime::Inf;
		}
	}

	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;
}

/*******************************************************************
* Function Name: receive
* Description: forward the init message to his childrens
********************************************************************/
ParallelProcessor &ParallelCoordinator::receive( const InitMessage &msg )
{

	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator cannot receive InitMessage. Must be received by either ParallelMCoordinator or ParallelSCoordinator!" ) ;
	MTHROW( e ) ;

}

/*******************************************************************
* Function Name: receive
* Description: sorts the external message queue and sends the @ message to
* the inminent childs
********************************************************************/
ParallelProcessor &ParallelCoordinator::receive( const CollectMessage &msg )
{
	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator cannot receive a CollectMessage. Must be received by either ParallelMCoordinator or ParallelSCoordinator!" ) ;
	MTHROW( e ) ;
}


/*******************************************************************
* Function Name: receive
* Description: sends the * message to the inminent child
********************************************************************/
ParallelProcessor &ParallelCoordinator::receive( const InternalMessage &msg )
{

	MASSERTMSG( doneCount() == 0, "Received an InternalMessage and doneCount is not cero!" );	

	//1. Sort the external message queue
	for( MessageBag::iterator extMsgs = externalMsgs.begin(); !(extMsgs == externalMsgs.end()); extMsgs++ )
	{
		sortExternalMessage( *((BasicExternalMessage*) (*extMsgs)));	
	}

	//2. Empty queue
	externalMsgs.eraseAll();	

	//3. Send all the internal messages
	ProcSet::const_iterator cursor;
	InternalMessage internal( msg.time(), id() ) ;

	for( cursor = synchronizeList.begin(); cursor != synchronizeList.end() ; cursor++ )
	{
		doneCount( doneCount() + 1);
		send( internal, *cursor ) ;
	}

	//Clear the synchronize set
	synchronizeList.erase( synchronizeList.begin(), synchronizeList.end());



	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: translates the output event to a X messages for the 
*              influenced children and to a Y message for his father
********************************************************************/
ParallelProcessor &ParallelCoordinator::receive( const BasicOutputMessage *msg )
{
	sortOutputMessage( *msg );
	delete msg;
	return *this;

}

/*******************************************************************
* Function Name: receive
* Description: recalculates the inminent child and sends it to his 
*              father inside a done message
********************************************************************/
ParallelProcessor &ParallelCoordinator::receive( const DoneMessage &msg )
{

	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator cannot receive DoneMessage. Must be received by either ParallelMCoordinator or ParallelSCoordinator!" ) ;
	MTHROW( e ) ;

}


/*******************************************************************
* Function Name: sortExternalMessage
* Description: send the External Message to the child's influences
********************************************************************/
ParallelCoordinator &ParallelCoordinator::sortExternalMessage( const BasicExternalMessage &msg )
{
	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator cannot sort a BasicExternalMessage. Must be done by either ParallelMCoordinator or ParallelSCoordinator!" ) ;
	MTHROW( e ) ;

}

/*******************************************************************
* Function Name: sortOutputMessage
* Description: send the External Message to the child's influences
*******************************************************************/
ParallelCoordinator &ParallelCoordinator::sortOutputMessage( const BasicOutputMessage &msg )
{
	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator cannot sort a BasicOutputMessage. Must be done by either ParallelMCoordinator or ParallelSCoordinator!" ) ;
	MTHROW( e ) ;

}

/*******************************************************************
* Function Name: calculateNextChange
* Description: calculate the time for the next change
********************************************************************/
VTime ParallelCoordinator::calculateNextChange(const VTime& time) const
{
	InvalidMessageException e ;
	e.addText( "The class ParallelCoordinator can not execute calculateNextChange!" ) ;
	MTHROW( e ) ;

}
/*******************************************************************
* Function Name: allocateState
********************************************************************/
BasicState* ParallelCoordinator::allocateState(){
	ParallelCoordinatorState *state = new ParallelCoordinatorState;
	state->dependants = new ParallelCoordinatorState::DependantList;
	return state;
}
