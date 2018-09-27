/*******************************************************************
*
*  DESCRIPTION: class ParallelSCoordinator
*
*  AUTHOR:	Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE:  24/01/2001
*
*******************************************************************/

/** include files **/
#include "pscoordin.h"       // header
#include "coupled.h"       // class Coupled
#include "pprocadm.h"	
#include "pmessage.h"
#include "parsimu.h"		//class ParallelMainSimulator

using namespace std;

/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelSCoordinator::ParallelSCoordinator( Coupled * coupled ) 
	: ParallelCoordinator(coupled)
{
	//The ParallelCoordinator constructor has already done much of the work
	parentId = coupled->masterId();
}

/*******************************************************************
* Function Name: initialize
********************************************************************/
void ParallelSCoordinator::initialize() 
{
	ParallelCoordinator::initialize();
	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;
}


/*******************************************************************
* Function Name: receive
* Description: sort the external messages and send the @ message to the inminent childs
********************************************************************/
ParallelProcessor &ParallelSCoordinator::receive( const CollectMessage &msg ) {

	//cout << "In function ParallelCoordinator::receive(CollectMessage)"<<endl<<flush;

	MASSERT( msg.time() == absoluteNext() );

	lastChange( msg.time() );
	nextChange( VTime::Zero );

	//1. Send collect message to inminent childs
	ParallelCoordinatorState::DependantList::const_iterator cursor;
	CollectMessage collect ( msg.time(), id() );

	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ ) 
	{
		if ( cursor->second == msg.time() )
		{
			synchronizeList.insert( cursor->first );
			doneCount( doneCount() + 1);
			send( collect , cursor->first );

		}//if

	}

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: receive an InitMessage
********************************************************************/
ParallelProcessor &ParallelSCoordinator::receive( const InitMessage &msg ) {

	//cout << "In function ParallelCoordinator::receive(InitMessage)"<<endl<<flush;

	//Send an init message to each of the dependants.

	doneCount ( dependants().size() );

	ParallelCoordinatorState::DependantList::const_iterator cursor;
	InitMessage init ( msg.time(), id() );

	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		this->send( init , cursor->first );

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: sends an X message to the port's influences
********************************************************************/
ParallelCoordinator &ParallelSCoordinator::sortExternalMessage( const BasicExternalMessage &msg )
{
	//cout << "In function ParallelSCoordinator::receive(ExternalMessage)"<<endl;

	const InfluenceList &influList( msg.port().influences() ) ;
	InfluenceList::const_iterator cursor( influList.begin() ) ;

	BasicExternalMessage xMsg( msg ) ;
	xMsg.procId( id() ) ;

	for( ; cursor != influList.end(); cursor++ ) {

		//If the destination model has a local master send message
		//to the local master, to the appropiate port
		//Otherwiser, ignore the message.
		if ((*cursor)->model().isLocalMaster()) {

			synchronizeList.insert( (*cursor)->model().localProc());
			xMsg.port( *(*cursor) ) ;
			send( xMsg,(*cursor)->model().localProc()  ) ;

		}//if
	}//for

	return *this ;

}	

/*******************************************************************
* Function Name: receive
* Description: recalculates the inminent child and sends it to his 
*              father inside a done message
********************************************************************/
ParallelProcessor &ParallelSCoordinator::receive( const DoneMessage &msg )
{
	//cout << "In function ParallelCoordinator::receive(DoneMessage)"<<endl << flush;	

	MASSERTMSG( doneCount() > 0, "Unexpected Done message!" );

	doneCount( doneCount() - 1);

	//Update the depdendant's absolute next time!
	dependants()[msg.procId()] = msg.time() + msg.nextChange();


	if( doneCount() == 0 )
	{
		lastChange ( msg.time() );

		nextChange( calculateNextChange( msg.time() ) );

		DoneMessage doneMsg( msg.time(), id(), nextChange(), true );

		send( doneMsg, parentId );

	}

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: translates the output event to a X messages for the 
*              influenced children and to a Y message for his father
********************************************************************/
ParallelCoordinator &ParallelSCoordinator::sortOutputMessage( const BasicOutputMessage &msg )
{

	bool sentToMaster = false;

	//cout << "In function ParallelSCoordinator::receive(OutputMessage)"<<endl << flush;
	Coupled &coupled( static_cast< Coupled & >( model() ) );

	const InfluenceList &influList( msg.port().influences() );

	InfluenceList::const_iterator cursor( influList.begin() );

	BasicOutputMessage outMsg( msg );
	outMsg.procId( id() );

	BasicExternalMessage extMsg;
	extMsg.time( msg.time() );
	extMsg.procId( id() );
	extMsg.value( msg.value()->clone() );
	extMsg.senderModelId( msg.port().modelId() );

	for( ; cursor != influList.end(); cursor++ )
	{
		// Check if the influenced port is an output port of the coupled model
		// If it is, then send the output message to the master processor
		// without translating the port.
		// If not found in the output ports list, then it must be an external message
		// If so, check if the recipient has a local master processor.
		// Otherwise, send the output message to the model's master (parentId)
		// Be carefull, only one output message should be sent to the master

		if( coupled.outputPorts().find( (*cursor)->id() ) == coupled.outputPorts().end() )
		{
			if ( (*cursor)->model().isLocalMaster()) {

				synchronizeList.insert( (*cursor)->model().localProc() );
				extMsg.port( * (*cursor) );
				send( extMsg, (*cursor)->model().localProc());

			} else {

				if ( !sentToMaster ) {

					send( outMsg, parentId );
					sentToMaster = true;
				}
			}
		} else
		{
			if ( !sentToMaster ) {
				send( outMsg, parentId );
				sentToMaster = true;
			}
		}
	}

	return *this;
}

/*******************************************************************
* Function Name: calculateNextChange
* Description: calculate the time for the next change
********************************************************************/
VTime ParallelSCoordinator::calculateNextChange(const VTime& time) const
{
	VTime next(VTime::Inf);

	ParallelCoordinatorState::DependantList::const_iterator cursor;

	//Check dependants
	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		if( cursor->second < next) next = cursor->second;

	next -= time;

	return next;

}
