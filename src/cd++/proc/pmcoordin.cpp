/*******************************************************************
*
*  DESCRIPTION: class ParallelMCoordinator
*
*  AUTHOR:	Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE:  16/11/2000
*
*******************************************************************/

/** include files **/
#include "pmcoordin.h"       // header
#include "coupled.h"       // class Coupled
#include "pprocadm.h"	
#include "pmessage.h"
#include "parsimu.h"		//class ParallelMainSimulator

using namespace std;

/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelMCoordinator::ParallelMCoordinator( Coupled * coupled ) 
	: ParallelCoordinator(coupled)
{

	//Processor is master, then parent is parent model's processor.
	parentId = coupled->parentId();

}


/*******************************************************************
* Function Name: initialize
********************************************************************/
void ParallelMCoordinator::initialize() 
{
	ParallelCoordinator::initialize();

	//Add the slave's processor list to the dependants list
	Coupled& coupled = static_cast<Coupled&> (model());

	for(ModelPartition::const_iterator mcursor = coupled.modelPartition().begin();
	mcursor != coupled.modelPartition().end(); mcursor++) {
		if ( id() != mcursor->second )
			//Add processor id to the slave list.
			slaves() [mcursor->second] = VTime::Inf ;
	}	

	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;
}


/*******************************************************************
* Function Name: receive
* Description: forward the init message to all dependants and slaves
********************************************************************/
ParallelProcessor &ParallelMCoordinator::receive( const InitMessage &msg )
{

	//cout << "In function ParallelMCoordinator::receive(InitMessage)"<<endl<<flush;

	//Send an init message to each of the dependants.

	doneCount ( dependants().size() + slaves().size());

	ParallelCoordinatorState::DependantList::const_iterator cursor;
	InitMessage init ( msg.time(), id() );

	//First, send Init message to slave coordinators
	for( cursor = slaves().begin() ; cursor != slaves().end() ; cursor++ )
		this->send( init , cursor->first );

	//Now, send to dependants.
	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		this->send( init , cursor->first );

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: sort the external messages and send the @ message to the inminent childs
********************************************************************/
ParallelProcessor &ParallelMCoordinator::receive( const CollectMessage &msg ) {

	//cout << "In function ParallelCoordinator::receive(CollectMessage)"<<endl<<flush;

	MASSERT( msg.time() == absoluteNext() );

	lastChange( msg.time() );
	nextChange( VTime::Zero );

	//1. Send collect message to inminent childs
	ParallelCoordinatorState::DependantList::const_iterator cursor;
	CollectMessage collect ( msg.time(), id() );

	//First, send message to slave coordinators
	for( cursor = slaves().begin() ; cursor != slaves().end() ; cursor++ ) 
	{
		if ( cursor->second == msg.time() )
		{
			synchronizeList.insert( cursor->first );
			doneCount( doneCount() + 1);
			send( collect , cursor->first );

		}//if

	}

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
* Description: sends an X message to the port's influences
********************************************************************/
ParallelCoordinator &ParallelMCoordinator::sortExternalMessage( const BasicExternalMessage &msg )
{
	//cout << "In function ParallelMCoordinator::receive(ExternalMessage)"<<endl << flush;

	ProcSet procs;	//Remote procs that have already received the message

	const InfluenceList &influList( msg.port().influences() ) ;
	InfluenceList::const_iterator cursor( influList.begin() ) ;

	BasicExternalMessage xMsg( msg ) ;
	xMsg.procId( id() ) ;

	for( ; cursor != influList.end(); cursor++ ) {

		//If the destination model has a local master send message
		//to the local master, to the appropiate port
		//Otherwise, find the slave processor that has to received the message
		//and forward the message. Only one message per slave should be sent.

		if ((*cursor)->model().isLocalMaster()) {

			synchronizeList.insert( (*cursor)->model().localProc() );
			xMsg.port( *(*cursor) ) ;
			send( xMsg,(*cursor)->model().localProc()  ) ;

		} else {

			//When forwarding a message, do not translate the port
			const ProcId& slaveId = (*cursor)->model().parentId();

			if (procs.find( slaveId ) == procs.end() ) {

				synchronizeList.insert( slaveId );
				procs.insert(slaveId);
				xMsg.port( msg.port());
				send( xMsg, slaveId );

			} // if

		}//for	

	}//else

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: translates the output event to a X messages for the 
*              influenced children and to a Y message for his father
********************************************************************/
ParallelCoordinator &ParallelMCoordinator::sortOutputMessage( const BasicOutputMessage &msg )
{

	//cout << "In function ParallelMCoordinator::receive(OutputMessage)"<<endl << flush;

	Coupled &coupled( static_cast< Coupled & >( model() ) );

	ProcSet procs;	//Remote procs that have already received an external msg.

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
		// Search for the port in the ouput ports list.
		// If found, send message to parent model.
		// If not found in the output ports list, then it must be an external message
		// If so, check if the recipient has a local master processor.
		// If the recipient does not have a local master processor, find the
		// corresponding slave processor and send the message.

		if( coupled.outputPorts().find( (*cursor)->id() ) == coupled.outputPorts().end() )
		{
			if ( (*cursor)->model().isLocalMaster()) {

				synchronizeList.insert((*cursor)->model().localProc());
				extMsg.port( * (*cursor) );
				send( extMsg, (*cursor)->model().localProc());

			} else {
				//When forwarding a message, do not translate the port

				//const ProcId& slaveId = parentSlaveForModel( (*cursor)->model() );
				const ProcId& slaveId = (*cursor)->model().parentId();

				//Two conditions must be satisfied for a message to be forwarded
				//to a slave processor:
				//1. The slave should not be the sender of the output message
				//2. The slave should not have already been sent the message

				if (slaveId != msg.procId() && procs.find( slaveId ) == procs.end() ) {

					synchronizeList.insert( slaveId );
					procs.insert(slaveId);
					extMsg.port( msg.port());
					send( extMsg, slaveId );

				} // if
			}
		} else
		{

			outMsg.port( * (*cursor) );
			send( outMsg, parentId );

		}
	}//for

	return *this;

}

/*******************************************************************
* Function Name: receive
* Description: recalculates the inminent child 
********************************************************************/
ParallelProcessor &ParallelMCoordinator::receive( const DoneMessage &msg )
{
	//cout << "In function ParallelCoordinator::receive(DoneMessage)"<<endl << flush;	

	MASSERTMSG( doneCount() > 0, "Unexpected Done message!" );

	doneCount( doneCount() - 1);

	//Update the depdendant's absolute next time!
	if (msg.isFromSlave()) {
		slaves()[msg.procId()] = msg.time() + msg.nextChange();
	} else {
		dependants()[msg.procId()] = msg.time() + msg.nextChange();
	}


	if( doneCount() == 0 )
	{
		lastChange ( msg.time() );

		nextChange( calculateNextChange( msg.time() ) );

		DoneMessage doneMsg( msg.time(), id(), nextChange(), false );

		send( doneMsg, parentId );

	}


	return *this;
}

/*******************************************************************
* Function Name: calculateNextChange
* Description: calculate the time for the next change
********************************************************************/
VTime ParallelMCoordinator::calculateNextChange(const VTime& time) const
{
	VTime next(VTime::Inf);

	ParallelCoordinatorState::DependantList::const_iterator cursor;

	//Check slaves
	for( cursor = slaves().begin() ; cursor != slaves().end() ; cursor++ )
		if( cursor->second < next) next = cursor->second;

	//Check dependants.
	for( cursor = dependants().begin() ; cursor != dependants().end() ; cursor++ )
		if( cursor->second < next) next = cursor->second;

	next -= time;

	return next;

}

/*******************************************************************
* Function Name: allocateState
********************************************************************/
BasicState* ParallelMCoordinator::allocateState(){
	ParallelMCoordinatorState *state = new ParallelMCoordinatorState;
	state->dependants = new ParallelCoordinatorState::DependantList;
	state->slaves = new ParallelCoordinatorState::DependantList;
	return state;
}
