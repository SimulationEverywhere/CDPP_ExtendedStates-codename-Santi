/*******************************************************************
*
*  DESCRIPTION: class ParallelProcessor ( Base class for ParallelCoordinator, 
*                                 ParallelSimulator and ParallelRootCoordinator )
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 06/11/2000
*
*******************************************************************/

#ifndef __PPROCESSOR_H
#define __PPROCESSOR_H

/** include files **/
#include <fstream>
#include "SimulationObj.h"		// SimulationObj
#include "pProcessorState.h"	//ProcessorState
#include "modelid.h"          	// class ModelId
#include "VTime.h"             // class Time
#include "except.h"
#include "value.h"
#include "msgbag.h"


/** forward declarations **/
class Model ;

class BasicMsgValue;
class Message;
class InitMessage ;      
class InternalMessage ;  
class BasicOutputMessage;
class OutputMessage ;   
class BasicExternalMessage;
class ExternalMessage ;   
class DoneMessage ;       
class CollectMessage;
class MessageBag;
class OutputSyncMessage;

class TWMessage;

class ParallelProcessorAdmin ;
class Port ;

/** declarations **/
typedef ModelId ProcId ;


class ParallelProcessor : public SimulationObj
{
public:
	virtual ~ParallelProcessor();	 // Destructor

	//Functions that are required by Warped
	void initialize();
	void executeProcess();
	BasicState* allocateState();

	// all the posible messages

	//All synchronization messages (Init, Internal, Done & Collect )
	//are received by &. There is no need to worry about memory deallocation since
	//there is code already written to handle that.
	//Instead, Input and Output messages are received through a pointer, and
	//will then be the owner of the pointer and will have to call delete
	//when finished!

	virtual ParallelProcessor &receive( const InitMessage &) ;
	virtual ParallelProcessor &receive( const InternalMessage &) ;
	virtual ParallelProcessor &receive( const DoneMessage &) ;
	virtual ParallelProcessor &receive( const CollectMessage &);
	virtual ParallelProcessor &receive( const OutputSyncMessage &);

	virtual ParallelProcessor &receive( const BasicOutputMessage *) ;
	virtual ParallelProcessor &receive( const BasicExternalMessage *) ;

	// the Processor Id 
	const ProcId &id() const
	{ return ident;}

	// return the asociate model
	Model &model() ;
	const Model &model() const ;

	const VTime &nextChange() const ;
	const VTime &lastChange() const ;
	const VTime absoluteNext() const {
		return nextChange() + lastChange();
	}

	const std::string asString() const ;

	virtual const std::string description() const;

	static const ProcId InvalidId ;

protected:
	friend class Model ;	
	friend class ParallelProcessorAdmin ;
	friend class ParallelRoot;

	ParallelProcessor() ;	 			// Default constructor
	ParallelProcessor( Model * ) ;   
	ParallelProcessor( const ParallelProcessor & );	//Copy constructor
	ParallelProcessor &operator =( const ParallelProcessor & ) ;	  // Assignment operator
	int operator ==( const ParallelProcessor & ) const ;	  // Equality operator

	ParallelProcessor &id( const ProcId & ) ;

	ParallelProcessor &nextChange( const VTime & ) ;
	ParallelProcessor &lastChange( const VTime & ) ;

	//Checks if a rollback has taken place and cleans
	//local variables.
	virtual bool rollbackCheck(const VTime& currentTime);

	// send
	virtual ParallelProcessor &send( const InitMessage &, const ProcId &dest  ) ;
	virtual ParallelProcessor &send( const InternalMessage &, const ProcId &dest ) ;
	virtual ParallelProcessor &send( const CollectMessage &, const ProcId &dest );
	virtual ParallelProcessor &send( const BasicOutputMessage &, const ProcId &dest ) ;
	virtual ParallelProcessor &send( const BasicExternalMessage &, const ProcId &dest) ;
	virtual ParallelProcessor &send( const DoneMessage &, const ProcId &dest) ;
	virtual ParallelProcessor &send( const OutputSyncMessage &, const ProcId &dest);

	virtual ParallelProcessor &sendOutput( const VTime &, const Port &, const AbstractValue & );
	virtual ParallelProcessor &sendOutput( const VTime &, const Port &, BasicMsgValue *);


	Model *pmodel();

	unsigned createlog;

	int logIndex;
	int numOutFiles;

	MessageBag externalMsgs;
	VTime lastMsgTime;

#ifndef KERNEL_TIMEWARP
	//If the NoTime Kernel is used, create a FileQueue.
	std::ostream** outFileQ;
#endif

private:

	void sendTWMessage(TWMessage*, const Message&, const ProcId &dest); //Sends a TWMessage
	void writelog( const VTime&, const std::string& );

	Model *mdl ;
	ProcId ident ;


	//VTime next ;	These are now part of the ParProcessorState
	//VTime last ; 


};	// class ParallelProcessor

/** inline **/
inline
	ParallelProcessor::ParallelProcessor() 
	: mdl( NULL )
{}

inline
	Model &ParallelProcessor::model()
{
	return *mdl ;
}

inline
	const Model &ParallelProcessor::model() const
{
	MASSERT( mdl ) ;
	return *mdl ;
}

inline
	const VTime &ParallelProcessor::nextChange() const
{
	return 	((ParallelProcessorState *)state->current)->next;
}

inline 
	const VTime &ParallelProcessor::lastChange() const
{
	return 	((ParallelProcessorState *)state->current)->last;
}

inline 
	ParallelProcessor &ParallelProcessor::nextChange( const VTime &t )
{
	((ParallelProcessorState *)state->current)->next = t ;
	return *this ;
}

inline
	ParallelProcessor &ParallelProcessor::lastChange( const VTime &t )
{
	((ParallelProcessorState *)state->current)->last = t ;
	return *this ;
}

inline
	const std::string ParallelProcessor::asString() const
{
	return description() + "(" + id() + ")" ;
}


inline
	Model *ParallelProcessor::pmodel()
{
	return mdl ;
}

#endif   //__PPROCESSOR_H 
