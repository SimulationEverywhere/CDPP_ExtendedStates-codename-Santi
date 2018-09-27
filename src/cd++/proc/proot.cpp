/*******************************************************************
*
*  DESCRIPTION: class ParallelRoot
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 05/01/2001
*
*******************************************************************/

/** include files **/
#include "proot.h"       // base
#include "pRootState.h"
#include "evaldeb.h"	// ShowVirtualTimeWhenFinish
#include "message.h"    // class Message
#include "pmodeladm.h"   // class SingleParallelModelAdm
#include "coupled.h"    // class Coupled
#include "parsimu.h"   // class ParallelMainSimulator
#include "log.h"	//class Log

using namespace std;

/** public **/
/*******************************************************************
* Function Name: Instance
* Description: 
********************************************************************/
ParallelRoot &ParallelRoot::Instance()
{
	return static_cast<ParallelRoot &>(
			SingleParallelProcessorAdmin::Instance().processor( ParallelProcessorAdmin::rootId) 
	) ; 
}

/*******************************************************************
* Function Name: Root
* Description: constructor Default
********************************************************************/
ParallelRoot::ParallelRoot( RootModel* rootmdl)
	: ParallelProcessor( rootmdl ) ,
topMasterId(ParallelProcessor::InvalidId)
{
	//Get the master processor for the top model and set it up as the child
	//processor.

	Coupled &coupled( static_cast<Coupled &>(SingleParallelModelAdm::Instance().model( "top" ) )) ;
	topMasterId = coupled.masterId();

}


/*******************************************************************
* Function Name: rootInitialize
* Description: clean event List and output device
********************************************************************/
ParallelRoot &ParallelRoot::rootInitialize()
{

	lastChange( VTime::Zero );
	nextChange( VTime::Inf );
	sendMsgType( ParallelRootState::CollectMsg );

	externalEvents.erase( externalEvents.begin(), externalEvents.end() ) ;
	stopTime( VTime::Inf ) ;

	return *this;
}

/*******************************************************************
* Function Name: rootSimulate
* Description: begin the simulation
********************************************************************/
ParallelRoot &ParallelRoot::rootSimulate()
{
	externalEvents.sort();
	eventsBegin();

	lastChange( VTime::Zero );
	InitMessage initMsg( VTime::Zero, id() ) ;
	this->send( initMsg, topMasterId ) ;
	return *this;
}

/*******************************************************************
* Function Name: rootStop
* Description: stop the simulation
********************************************************************/
ParallelRoot &ParallelRoot::rootStop()
{
	if (ShowVirtualTimeWhenFinish().Active())
		cerr << "\n" << lastChange() << "\n";

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: send the time and value of the message to the output device 
********************************************************************/
ParallelProcessor &ParallelRoot::receive( const BasicOutputMessage *msg )
{
	//If an output file has been opened
	if (createOutput) {

		string line = msg->time().asString() + " " + msg->port().name() + " " + msg->value()->asString() + "\n";

#ifdef KERNEL_TIMEWARP

		FileData *data = new FileData;
		data->time = msg->time();

		data->length = line.length()+1;
		data->line = new char[line.length()+1];
		line.copy( data->line, string::npos);
		data->line[line.length()] = 0;

		outFileQ[outputIndex].insert(data);
#else
		(*outFileQ[outputIndex]) << line;
		outFileQ[outputIndex]->flush();
#endif

	}

	delete msg;

	return *this;
}

/*******************************************************************
* Function Name: receive
* Description: advance the time and go on with the simulation
********************************************************************/
ParallelProcessor  &ParallelRoot::receive( const DoneMessage &msg )
{

	if ( lastChange() < msg.time() )
	{
		cerr << "At " << msg.time().asString() << "\r";

	}


	//We have to find out if the DoneMessage corresponds to the
	//a response to a CollectMessage or to a InternalMessage

	lastChange( msg.time() );
	nextChange( msg.nextChange() );

	//If we have to send an Internal Message, send it now!
	if ( sendMsgType() == ParallelRootState::InternalMsg ) 
	{
		//It should be the case that absoluteNext == msg.time();
		InternalMessage intMsg( absoluteNext(), id());
		send ( intMsg, topMasterId );

		//Calculate GVT
		//Set the next message type to Collect
		sendMsgType ( ParallelRootState::CollectMsg );
	} 

	else
		//See if there are any external events to send before the collect
	{

		if( endOfEvents() && nextChange()==VTime::Inf )
			rootStop() ;

		else

		{
			// is there any event?
			Event ev( VTime::Inf ) ;

			if( !endOfEvents() )
				ev = *(currentEvent()) ;


			VTime nt( absoluteNext() );

			//If there is an external event prior to an /at the same time as
			//an Internal transition then send the external messages
			//before sending the CollectMessage
			if( ev.time < absoluteNext() )
			{
				nt = ev.time;

			} //if



			//Should we go on?
			if ( stopTime() < nt )
				rootStop() ;

			else
			{
				//Send all the external msgs.
				while ( !endOfEvents() && ev.time == nt ) 
				{
					ExternalMessage extMsg( ev.time, id(), *ev.port, *ev.value );
					send( extMsg, topMasterId );
					eventsMoveNext() ;

					if ( !endOfEvents() )
						ev = *(currentEvent()) ;		
				} //while

				//Aca podria venir el while
				//...
				//...
				
				//If we have also an internal transition, then send the Collect message

				if ( nt == absoluteNext() )
				{
					CollectMessage collectMsg = CollectMessage( nt, id() ) ;

					//Set the next message type to Internal		
					sendMsgType ( ParallelRootState::InternalMsg );

					send ( collectMsg, topMasterId );
				}

				else
				{
					//Set the time for next change to now.
					lastChange( nt );
					nextChange( VTime::Zero );

					InternalMessage intMsg( absoluteNext(), id());
					send ( intMsg, topMasterId );

					//Set the next message type to Collect
					sendMsgType ( ParallelRootState::CollectMsg );

				}
			}//if

		}//if stop

	}//if sendMsgType == Internal

	return *this;
}

/*******************************************************************
* Function Name: addExternalEvent
* Description: add external event
********************************************************************/
ParallelRoot &ParallelRoot::addExternalEvent( const VTime &t, const Port &p, const AbstractValue &v )
{
	externalEvents.push_back( Event(t, &p, v) );
	return *this;
}

/*******************************************************************
*Functions that are required by Warped
********************************************************************/

/*******************************************************************
* Function Name: initialize
********************************************************************/
void ParallelRoot::initialize() {

	//Create file queues

	numOutFiles = 0;

	if (Log::Default.createLog() ) {
		numOutFiles++;
		createlog = Log::Default.logType();
		logIndex = numOutFiles - 1;
	} else {
		createlog = Log::logNone;
	}

	if (ParallelMainSimulator::Instance().outputName() != "/dev/null" ) {
		numOutFiles++;
		createOutput = true;
		outputIndex = numOutFiles - 1;
	} else {
		createOutput = false;
	}

#ifdef KERNEL_TIMEWARP
	//If the TimeWarp kernel is being used
	if ( Log::Default.createLog() && createOutput) {
		outFileQ = new FileQueue[2];
	
		if (Log::Default.logToStdOut() )
			outFileQ[logIndex].open(string(Log::Default.filename() + id()).c_str());
		else
			outFileQ[logIndex].open( "/dev/stdout");

		outFileQ[outputIndex].open(ParallelMainSimulator::Instance().outputName().c_str());

	} else if ( Log::Default.createLog() ) {

		if (Log::Default.logToStdOut() )
			outFileQ[0].open(string(Log::Default.filename() + id()).c_str());
		else
			outFileQ[0].open("dev/stdout");

	} else if ( createOutput ) {
		outFileQ = new FileQueue[0]( ParallelMainSimulator::Instance().outputName().c_str());
	}

#else
	//If the NoTime kernel is being used
if (id()<=800){ //sixuan
	if ( Log::Default.createLog() && createOutput) {
		outFileQ = new ostream*[2];

		if (Log::Default.logToStdOut() )
			outFileQ[logIndex] = &cout;

		else
			
			outFileQ[logIndex] = new fstream(string(Log::Default.filename() + id()).c_str(), ios::out);

		outFileQ[outputIndex] = new fstream (ParallelMainSimulator::Instance().outputName().c_str(), ios::out);

	} else if ( Log::Default.createLog() ) {
		outFileQ = new ostream*[1];

		if (Log::Default.logToStdOut() )
			outFileQ[0] = &cout; 
		else
			outFileQ[0] = new fstream(string(Log::Default.filename() + id()).c_str(), ios::out);

	} else if ( createOutput ) {
		outFileQ = new ostream*[1];
		outFileQ[0] = new fstream( ParallelMainSimulator::Instance().outputName().c_str(), ios::out);
	}
}//sixuan
#endif

	rootInitialize();
	ParallelMainSimulator::Instance().debugStream() << "OK" << endl << flush;

}
