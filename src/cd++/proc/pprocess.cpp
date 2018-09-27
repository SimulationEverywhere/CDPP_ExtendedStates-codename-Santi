/*******************************************************************
*
*  DESCRIPTION: class ParallelProcessor
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 07/11/2000
*
*******************************************************************/

/** include files **/
#include "strutil.h"
#include "pprocess.h"    		// header
#include "pmessage.h"
#include "model.h"      		// class Model
#include "parsimu.h"
#include "msgbag.h"
#include "log.h"			// class Log

using namespace std;

/** public data **/
const ProcId ParallelProcessor::InvalidId( -1 ) ;

/** public functions **/
/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelProcessor::ParallelProcessor( Model *m )
	: lastMsgTime( VTime::Zero )
	,mdl( m )
	, ident( InvalidId ) 

{
	MASSERT( m );
}

/*******************************************************************
* Function Name: Destructor
********************************************************************/
ParallelProcessor::~ParallelProcessor()
{

#ifndef KERNELTIMEWARP
	//Close all open files...
	for( int i = 0; i < numOutFiles; i++)
		delete outFileQ[i];

	delete outFileQ;

#endif	

}

/*******************************************************************
* Function Name: id
********************************************************************/
ParallelProcessor &ParallelProcessor::id( const ProcId &id )
{
	//DEVS processor Id
	ident = id ;

	//Warped Object Id
	SimulationObj::id = id;

	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const InitMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive InitMessage!" ) ;
	MTHROW( e ) ;
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const CollectMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive InternalMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const InternalMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive InternalMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const OutputSyncMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive OutputSyncMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const BasicOutputMessage * )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive OutputMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const BasicExternalMessage *msg)
{

	externalMsgs.add( msg );

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
ParallelProcessor &
	ParallelProcessor::receive( const DoneMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive DoneMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Method: sendOutput
********************************************************************/
ParallelProcessor &
	ParallelProcessor::sendOutput( const VTime &time, const Port &port, const AbstractValue &value )
{

	send( OutputMessage( time, id(), port, value ), model().parentId() ) ;
	return *this;
}

/*******************************************************************
* Method: sendOutput
********************************************************************/
ParallelProcessor &
	ParallelProcessor::sendOutput( const VTime & time, const Port & port , BasicMsgValue *value) 
{
	send( BasicOutputMessage( time, id(), port, value ), model().parentId() ) ;
	return *this;
}


/*******************************************************************
* Function Name: description
********************************************************************/
const string 
	ParallelProcessor::description() const
{
	return mdl->description();
}

/*******************************************************************
* Function Name: rollbackCheck
********************************************************************/
bool ParallelProcessor::rollbackCheck( const VTime& currentTime )
{

	bool rollback;

	if ( currentTime < lastMsgTime ) 
	{
		externalMsgs.eraseAll();
		rollback = true;
		cout << "Rolling back, current time: " << currentTime << " last time: " << lastMsgTime << endl;
	} 
	else
	{
		rollback = false;
	}

	lastMsgTime = currentTime;

	return rollback;
}


//Functions that are required by Warped
/*******************************************************************
* Function Name: initialize
********************************************************************/
void 
ParallelProcessor::initialize() {
	ParallelMainSimulator::Instance().debugStream() << "Initializing Object " << description() << "(" << id() << "): " << flush;

#ifdef KERNEL_TIMEWARP

	//For the TimeWarp Kernel, use the File API provided by TimeWarp
	if (Log::Default.createLog() ) {
		numOutFiles = 1;
		createlog = Log::Default.logType();
		logIndex = 0;

		if ( !Log::Default.logToStdOut() )
			outFileQ = new FileQueue[1]( string(Log::Default.filename() + id()).c_str());
		else
			outFileQ = new FileQueue[1]( "/dev/stdout" );

	} else {
		createlog = Log::LogType::logNone;
	}

#else
	//For the NoTime kernel, use ofstreams
	if ( Log::Default.createLog() ) {
		numOutFiles = 1;
		createlog = Log::Default.logType();;
		logIndex = 0;

		if ( !Log::Default.logToStdOut() ) {
			//sixuan: bug in 32bit system, cannot open stream more than 1000, here control to MAX 800
			if (id()<=100){
				outFileQ = new ostream*[1];
				outFileQ[0] = new fstream( string(Log::Default.filename() + id()).c_str(), ios::out );
			}
		}
		else
		{
			outFileQ = new ostream*[1];
			outFileQ[0] = &cout;

		}
	} else {
		createlog = Log::logNone;
	}

#endif
}

/*******************************************************************
* Function Name: executeProcess
********************************************************************/
void 
ParallelProcessor::executeProcess() {

	//Receive an event and execute the corresponding function.
	TWMessage* receivedMsg;
	receivedMsg = (TWMessage*) getEvent();

	if (receivedMsg != NULL) {
		switch (receivedMsg->getMessageType()) {

			case TWMessage::TWInitMsg:
				{

					InitMessage *msg = ((TWInitMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logInit) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( *msg );
					delete msg;
					break;
				}

			case TWMessage::TWInternalMsg:
				{
					InternalMessage *msg = ((TWInternalMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logInternal) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( *msg );
					delete msg;
					break;
				}

			case TWMessage::TWOutputSyncMsg:
				{
					OutputSyncMessage *msg = ((TWOutputSyncMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logOutput) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( *msg );
					delete msg;
					break;
				}	

			case TWMessage::TWCollectMsg:
				{
					CollectMessage *msg = ((TWCollectMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logCollect ) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( *msg );
					delete msg;
					break;
				}

			case TWMessage::TWDoneMsg:
				{
					DoneMessage *msg = ((TWDoneMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logDone ) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( *msg );
					delete msg;
					break;	
				}

			case TWMessage::TWExternalMsg:
				{
					BasicExternalMessage *msg = ((TWExternalMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logExternal ) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( msg );
					break;	
				}

			case TWMessage::TWOutputMsg:
				{
					BasicOutputMessage *msg = ((TWOutputMessage *)receivedMsg)->getMessage();

					if (createlog & Log::logOutput ) {
						string log =  msg->asStringReceived() + " / " + model().description() + "(" + id() + ")" + "\n";
						writelog ( msg->time(), log ); 
					}

					rollbackCheck( msg->time());
					receive( msg );
					break;				
				}
		}

	} else {
		cout << "NULL MESSAGE!!" << endl;
	}
}

/*******************************************************************
* Function Name: allocateState
********************************************************************/
BasicState* 
ParallelProcessor::allocateState() {
	return new ParallelProcessorState();
}

/*******************************************************************
* Function Name: sendTWMessage
********************************************************************/
void ParallelProcessor::sendTWMessage(TWMessage* twmsg, const Message& msg, const ProcId &dest) {

	twmsg->dest = dest;	

	if (createlog & Log::logSent ) {
		string log = msg.asStringSent(dest) + "\n";
		writelog( msg.time(),log ); 
	}


	sendEvent( twmsg );
}



/*******************************************************************
* Function Name: send
********************************************************************/
ParallelProcessor &
ParallelProcessor::send( const InitMessage &msg, const ProcId &dest ) {

	TWInitMessage* newMsg = new TWInitMessage(msg);
	sendTWMessage( newMsg, msg, dest );
	return *this;
}

ParallelProcessor &
ParallelProcessor::send( const InternalMessage &msg, const ProcId& dest ) {

	TWInternalMessage* newMsg = new TWInternalMessage(msg);
	sendTWMessage( newMsg, msg, dest );
	return *this;
}

ParallelProcessor &
ParallelProcessor::send( const OutputSyncMessage &msg, const ProcId &dest) {

	TWOutputSyncMessage* newMsg = new TWOutputSyncMessage(msg);
	sendTWMessage( newMsg, msg, dest );
	return *this;	
}

ParallelProcessor &
ParallelProcessor::send( const CollectMessage &msg, const ProcId& dest ) {

	TWCollectMessage* newMsg = new TWCollectMessage(msg);
	sendTWMessage( newMsg, msg, dest );
	return *this;
}

ParallelProcessor &
ParallelProcessor::send( const BasicOutputMessage &msg, const ProcId &dest ) {

	int msgSize = sizeof(TWBasicOutputMessage) + msg.value()->valueSize();

	TWOutputMessage* newMsg = (TWOutputMessage*) new char[msgSize];
	new (newMsg) TWOutputMessage(msg);

	sendTWMessage( newMsg, msg, dest );
	return *this;
}

ParallelProcessor &
ParallelProcessor::send( const BasicExternalMessage &msg, const ProcId &dest) {

	int msgSize = sizeof(TWBasicExternalMessage) + msg.value()->valueSize();

	TWExternalMessage* newMsg = (TWExternalMessage*) new char[msgSize];
	new (newMsg) TWExternalMessage(msg);

	sendTWMessage( newMsg, msg, dest );
	return *this;
}

ParallelProcessor &
ParallelProcessor::send( const DoneMessage &msg, const ProcId &dest) {

	TWDoneMessage* newMsg = new TWDoneMessage(msg);
	sendTWMessage( newMsg, msg, dest );
	return *this;
}

/*******************************************************************
* Function Name: writelog
********************************************************************/
void
ParallelProcessor::writelog(const VTime& time, const string& line) {
//sixuan
if (id()<=100){

	//Precondition: A log file has been created

#ifdef KERNEL_TIMEWARP
	FileData *data = new FileData;
	data->time = time;

	data->length = line.length() + 1;
	data->line = new char[line.length()+1];
	line.copy( data->line, line.length());
	data->line[line.length()] = 0;

	outFileQ[logIndex].insert(data);

#else
	//If the NoTime kernel is used
	*(outFileQ[logIndex]) << line << flush;
#endif
}

}
