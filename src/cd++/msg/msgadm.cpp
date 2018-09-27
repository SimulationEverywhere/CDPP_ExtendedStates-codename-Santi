/*******************************************************************
*
*  DESCRIPTION: class MessageAdmin, SingleMsgAdm
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 8/10/1998 (v2)
*
*******************************************************************/

/** include files **/
#include "msgadm.h"           // base header
#include "parsimu.h"         // class Simulator
#include "procadm.h"

using namespace std;

/** private data **/
MessageAdmin *SingleMsgAdm::instance( NULL ) ;

/*******************************************************************
* Function Name: ~MessageAdmin
********************************************************************/
MessageAdmin::~MessageAdmin()
{}

/*******************************************************************
* Function Name: send
* Description: add a message to the queue 
********************************************************************/
MessageAdmin &MessageAdmin::send( const Message &msg, const ModelId &id )
{
	unprocessedQueue.push_back( UnprocessedMsg( msg.clone(), id ) ) ;
	return *this ;
}

/*******************************************************************
* Function Name: run
********************************************************************/
MessageAdmin &MessageAdmin::run()
{
	running = true ;

	while( isRunning() &&  !unprocessedQueue.empty() )
	{
		const UnprocessedMsg &umsg( unprocessedQueue.front() ) ;
		Processor &proc( SingleProcessorAdmin::Instance().processor( umsg.second ) ) ;

		ParallelMainSimulator::Instance().logStream() << "Mensaje " << umsg.first->asString() << " para " << proc.asString() << "\n"; 

		umsg.first->sendTo( proc ) ;
		delete umsg.first ;
		unprocessedQueue.pop_front() ;
	}
	return *this ;
}

/*******************************************************************
* Function Name: stop
********************************************************************/
MessageAdmin &MessageAdmin::stop()
{
	running = false ;
	return *this ;
}

/** private functions **/
/*******************************************************************
* Function Name: MessageAdmin
* Description: Constructor
********************************************************************/
MessageAdmin::MessageAdmin()
: running( false )
{}
