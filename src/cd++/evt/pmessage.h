/*******************************************************************
*
*  DESCRIPTION: definitions of messages ( Y, *, D, X, I )for the TimeWarp version
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar

*
*  DATE: 01/11/2000
*
*******************************************************************/

#ifndef __PMESSAGE_H
#define __PMESSAGE_H


/** include files **/
#include "BasicEvent.h"	 // TimeWarp BasicEvent class
#include "message.h"	 	// NCD++ Message.
#include "value.h"
#include "port.h"

#include <cstring>

/** definitions **/
class TWMessage: public BasicEvent 
{
public:

	//Proc Id of the sender
	ProcId procId;

	//define the different message types
	enum TWMessageType { 
		TWInitMsg, 
		TWInternalMsg, 
		TWDoneMsg, 
		TWExternalMsg, 
		TWOutputMsg,
		TWOutputSyncMsg,
		TWCollectMsg };

	TWMessageType getMessageType(){
		return msgType;}

	virtual Message* getMessage() = 0;

	virtual ~TWMessage(){}	//Destructor

protected:

	TWMessage( const Message &msg, const TWMessageType type): msgType(type)  {
		procId = msg.procId();
		recvTime = msg.time();

		//size should be initialized in each subclass
	};

private:
	TWMessageType msgType;	

};	// class TWMessage



class TWInitMessage : public TWMessage
{
public:

	TWInitMessage( const InitMessage &msg ): TWMessage(msg, TWInitMsg){
		size = sizeof(TWInitMessage);
	};	// constructor


	InitMessage* getMessage();

	virtual ~TWInitMessage()	//Destructor
	{}

};

class TWInternalMessage : public TWMessage
{
public:

	TWInternalMessage( const InternalMessage &msg ): TWMessage(msg , TWInternalMsg) {
		size = sizeof(TWInternalMessage);
	};	// constructor

	virtual ~TWInternalMessage()	//Destructor
	{}

	virtual InternalMessage* getMessage();

};	// class TWInternalMessage

class TWOutputSyncMessage : public TWMessage
{
public:

	TWOutputSyncMessage( const OutputSyncMessage &msg ): TWMessage(msg , TWOutputSyncMsg) {
		size = sizeof(TWOutputSyncMessage);
	};	// constructor

	virtual ~TWOutputSyncMessage()	//Destructor
	{}

	virtual OutputSyncMessage* getMessage();

};	// class TWOutputSyncMessage

class TWCollectMessage : public TWMessage
{
public:

	TWCollectMessage( const CollectMessage &msg ): TWMessage(msg , TWCollectMsg) {
		size = sizeof(TWCollectMessage);
	};	// constructor

	virtual ~TWCollectMessage()	//Destructor
	{}

	virtual CollectMessage* getMessage();

};	// class TWCollectMessage

class TWDoneMessage : public TWMessage
{
public:

	TWDoneMessage( const DoneMessage &msg ): TWMessage(msg,  TWDoneMsg) {
		nextChange = msg.nextChange() ;
		slaveSync = msg.isFromSlave() ;
		size = sizeof(TWDoneMessage);
	};	// constructor

	virtual ~TWDoneMessage()	//Destructor
	{}

	virtual DoneMessage* getMessage();

private:
	VTime nextChange;	
	bool slaveSync;

};	// class TWDoneMessage


//This class is here to guarantee that the memory copying in
//TWExternalMessage will work correctly.
class TWBasicExternalMessage : public TWMessage
{
public:
	TWBasicExternalMessage( const BasicExternalMessage &msg) : TWMessage( msg, TWExternalMsg )
	{	mid = msg.port().modelId();
		p = msg.port().id();
		sendermid = msg.senderModelId();
		val = msg.value()->v;
	};
protected:
	ModelId mid;
	ModelId sendermid;
	PortId p;
	value_ptr val;
}; //class TWBasicExternalMessage

class TWExternalMessage : public TWBasicExternalMessage
{
public:

	TWExternalMessage( const BasicExternalMessage &msg ) : TWBasicExternalMessage(msg) {
		size = sizeof(TWBasicExternalMessage) + msg.value()->valueSize();

		//Dirty memory copying...
		//memcpy((BasicMsgValue*)this, msg.value(), msg.value()->valueSize());

	};	// constructor

	virtual ~TWExternalMessage()	//Destructor
	{}

	virtual BasicExternalMessage* getMessage();

};	// class TWExternalMessage



//TWOutputMessage will work correctly.
class TWBasicOutputMessage : public TWMessage
{
public:
	TWBasicOutputMessage( const BasicOutputMessage &msg) : TWMessage( msg, TWOutputMsg )
	{	mid = msg.port().modelId();
		p = msg.port().id();
		val = msg.value()->v;
	};
protected:
	ModelId mid;
	PortId p;
	value_ptr val;
}; //class TWBasicOutputMessage

class TWOutputMessage : public TWBasicOutputMessage
{
public:

	TWOutputMessage( const BasicOutputMessage &msg ) : TWBasicOutputMessage(msg) {
		size = sizeof(TWBasicOutputMessage) + msg.value()->valueSize();

		//Dirty memory copying...
		//memcpy((BasicMsgValue*)this, msg.value(), msg.value()->valueSize());

	};	// constructor

	virtual ~TWOutputMessage()	//Destructor
	{}

	virtual BasicOutputMessage* getMessage();

};	// class TWOutputMessage


#endif   //__PMESSAGE_H 
