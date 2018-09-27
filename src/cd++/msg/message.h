/*******************************************************************
*
*  DESCRIPTION: definitions of messages ( Y, *, @, D, X, I )
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 25/4/1999 (v2)
*  DATE: 16/2/2001 (v3)
*
*******************************************************************/

#ifndef __MESSAGE_H
#define __MESSAGE_H

/** include files **/
#include <sstream>

#include "model.h"
#include "VTime.h"   	// class VTime
#include "value.h"  	 	// class Value
#include "parsimu.h"		// class ParallelMainSimulator
#include "process.h"     	// class Processor, ProcId
#include "pprocess.h"	
#include "pprocadm.h"	 // ParallelProcessorAdmin
#include "port.h"
#include "strutil.h"
#include "cellpos.h"
#include "tuple_value.h"
#include "value.h"

#include <cassert>

/** foward declarations **/
class Port  ;
class ParallelProcessor ;

/** definitions **/
/** class BasicMsgValue **/
//BasicMsgValue is the class that holds the value for an
//either an ExternalMsg or an OutputMsg

class BasicMsgValue 
{
public:
	BasicMsgValue();
	virtual ~BasicMsgValue();

	virtual int valueSize() const;

	virtual std::string asString() const;

	virtual BasicMsgValue* clone() const = 0;

	BasicMsgValue(const BasicMsgValue& );

	BasicMsgValue(const AbstractValue &val) : v(AbstractValue::to_value_ptr(val)) {}

	static BasicMsgValue *build_from(const AbstractValue&);

	value_ptr v;
};

/** class RealMsgValue **/
//RealMsgValue is the class that holds the value
//for messages with a real value.
class RealMsgValue : public BasicMsgValue
{
public:
	RealMsgValue();
	RealMsgValue( const Value& val);
	~RealMsgValue() {};

	int valueSize() const; 

	std::string asString() const ;

	BasicMsgValue* clone() const;

	RealMsgValue(const RealMsgValue& );
};


template<class T>
class TupleMsgValue : public BasicMsgValue
{
public:
	~TupleMsgValue() {};

	TupleMsgValue() : BasicMsgValue() {};
	TupleMsgValue(const Tuple<T>& val) : BasicMsgValue(val) {};
	TupleMsgValue(const TupleMsgValue &msg)
	{
		this->v = msg.v;
	};

	int valueSize() const
	{
		const tuple_ptr<Real> t = std::dynamic_pointer_cast<Tuple<Real> >(this->v);
		return t->size();
	};

	std::string asString() const
	{
		std::ostringstream stream;
		const tuple_ptr<Real> t = std::dynamic_pointer_cast<Tuple<Real> >(this->v);

		t->print(stream);

		return stream.str();
	};

	BasicMsgValue* clone() const
	{
		return new TupleMsgValue(*this);
	};
};


/** class Message **/
class Message
{
public:
	virtual ~Message()	//Destructor
	{}

	//** Queries **//
	const VTime &time() const
	{return t;}

	const ProcId &procId() const
	{ return proc; }

	//** Modifiers **//
	Message &time( const VTime &time )
	{ t = time; return *this; }

	Message &procId( const ProcId &p )
	{ proc = p; return *this; }

	virtual Message *clone() const = 0 ;

	virtual const std::string type() const = 0 ;

	virtual const std::string asString() const;
	virtual const std::string asStringReceived() const;
	virtual const std::string asStringSent( const ProcId& dest ) const;



protected:                                                
	Message( const VTime &time, const ProcId &id )	// constructor
	: t(time)
		, proc(id)
	{}

	Message( const Message &msg)			// Copy constructor
		: t(msg.t)
		, proc(msg.proc)
	{}

	Message& operator=( const Message& );						 // Assignment operator
	int operator==( const Message& ) const;					 // Equality operator

private:
	VTime t;
	ProcId proc;

};	// class Message



class InitMessage : public Message
{
public:
	InitMessage( const VTime &tm, const ProcId &m)  // Default constructor
	:Message( tm, m)
	{}

	virtual Message *clone() const
	{ return new InitMessage(*this);}

	virtual const std::string type() const
	{return "I" ;}
};	// class InitMessage

class OutputSyncMessage : public Message
{
public:
	OutputSyncMessage( const VTime &tm = VTime::Zero, const ProcId &m = 0 )	//Default constructor
	:Message(tm,m)
	{}

	virtual Message *clone() const
	{return new OutputSyncMessage(*this);}

	virtual const std::string type() const
	{return "$" ;}

protected:
	OutputSyncMessage& operator=(const OutputSyncMessage&);			  //Assignment operator
	int operator==(const OutputSyncMessage&) const;					  //Equality operator

private:

};


class InternalMessage : public Message
{
public:
	InternalMessage( const VTime &tm = VTime::Zero, const ProcId &m = 0 )	//Default constructor
	:Message(tm,m)
	{}

	virtual Message *clone() const
	{return new InternalMessage(*this);}

	virtual const std::string type() const
	{return "*" ;}

protected:
	InternalMessage& operator=(const InternalMessage&);			  //Assignment operator
	int operator==(const InternalMessage&) const;					  //Equality operator

private:

};	// class InternalMessage

class CollectMessage : public Message
{
public:
	CollectMessage( const VTime &tm = VTime::Zero, const ProcId &m = 0 )	//Default constructor
	:Message(tm,m)
	{}

	virtual CollectMessage *clone() const
	{return new CollectMessage(*this);}

	virtual const std::string type() const
	{return "@" ;}

protected:
	CollectMessage& operator=(const CollectMessage&);			  //Assignment operator
	int operator==(const CollectMessage&) const;					  //Equality operator

private:

};	// class CollectMessage



class DoneMessage : public Message
{
public:
	DoneMessage();//Default constructor
	DoneMessage( const VTime &tm, const ProcId &m, const VTime &nextChange, bool fromSlave )
	:Message(tm,m) ,
	next(nextChange) ,
	slaveSync( fromSlave)
	{}

	const VTime &nextChange() const
	{return next;}

	Message &nextChange( const VTime &time )
	{next = time; return *this;}

	bool isFromSlave() const
	{return slaveSync;}

	Message &isFromSlave( bool fromSlave ) 
	{ slaveSync = fromSlave; return *this;}

	virtual Message *clone() const
	{return new DoneMessage(*this);}

	virtual const std::string type() const
	{return "D" ;}

	virtual const std::string asString() const; 

protected:
	DoneMessage(const DoneMessage &msg)	  //Copy constructor
		:Message(msg)
		,next(msg.next)
	{}

	DoneMessage& operator=(const DoneMessage&);						  //Assignment operator
	int operator==(const DoneMessage&) const;							  //Equality operator

private:
	VTime next;
	bool slaveSync;

};	// class DoneMessage

class BasicPortMessage : public Message
{

public:
	virtual ~BasicPortMessage() 
	{ if ( v ) delete v;}

	BasicPortMessage()
		:Message(VTime::Zero, ParallelProcessor::InvalidId)
	,userDefined( true )
		,p(NULL) 
		,v(NULL)
	{}

	BasicPortMessage(const VTime &tm, const ProcId &m, const Port &port, BasicMsgValue* val )
	:Message(tm,m)
	,userDefined( true )
		,p(&port)
	,v(val) {}

	BasicPortMessage(const BasicPortMessage &msg)	//Copy constructor
		:Message(msg)
		,userDefined(msg.userDefined)
		,p(msg.p)
		,v(msg.v->clone())
	{}

	const BasicMsgValue *value() const
	{return v;}

	Message &value( const BasicMsgValue *val )
		{ if (v) delete v; v=val; return *this;}

	const Port &port() const
	{assert(p); return *p;}

	Message &port( const Port &port )
	{p = &port; return *this;}

	virtual const std::string asString() const;


protected:
	BasicPortMessage& operator=(const BasicPortMessage&);	//Assignment operator
	int operator==(const BasicPortMessage&) const;	//Equality operator

	bool userDefined;

private:
	const Port *p;
	const BasicMsgValue *v;
};

class BasicExternalMessage : public BasicPortMessage
{
public:

	BasicExternalMessage()
	{}

	BasicExternalMessage(const VTime &tm, const ProcId &m, const Port &port, BasicMsgValue* val, const ModelId& senderId )
	:BasicPortMessage(tm,m, port, val), sender(senderId)
	{}

	BasicExternalMessage(const VTime &tm, const ProcId &m, const Port &port, BasicMsgValue* val )
	:BasicPortMessage(tm,m, port, val)
	{}

	BasicExternalMessage(const BasicExternalMessage &msg)	//Copy constructor
		:BasicPortMessage(msg)
	{
		sender = msg.sender;
	}

	virtual Message *clone() const
	{return new BasicExternalMessage(*this);}

	virtual const std::string type() const
	{return "X" ;}

	Message& senderModelId( const ModelId& model)
	{sender = model; return *this;}

	const ModelId& senderModelId() const
	{return sender;}

protected:
	BasicExternalMessage& operator=(const BasicExternalMessage&);	//Assignment operator
	int operator==(const BasicExternalMessage&) const;	//Equality operator

	ModelId sender;

}; /** class BasicExternalMessage **/

class ExternalMessage : public BasicExternalMessage
{
public:
	ExternalMessage()
	{ userDefined = false; }

	ExternalMessage( const VTime &tm, const ProcId &m, const Port &port, const AbstractValue &val )
	:BasicExternalMessage(tm,m, port, BasicMsgValue::build_from(val))
	{}

	ExternalMessage(const ExternalMessage &msg)	//Copy constructor
		:BasicExternalMessage(msg)
	{}

	const value_ptr &value() const
	{return (BasicExternalMessage::value())->v;}

	Message &value( const AbstractValue &val )
	{BasicExternalMessage::value(BasicMsgValue::build_from(val)); return *this;}

	virtual Message *clone() const
	{return new ExternalMessage(*this);}

protected:
	ExternalMessage& operator=(const ExternalMessage&);	//Assignment operator
	int operator==(const ExternalMessage&) const;	//Equality operator

};	// class ExternalMessage


class BasicOutputMessage : public BasicPortMessage
{
public:

	BasicOutputMessage()
	{}

	BasicOutputMessage(const VTime &tm, const ProcId &m, const Port &port, BasicMsgValue* val )
	:BasicPortMessage(tm,m, port, val)
	{}

	BasicOutputMessage(const BasicOutputMessage &msg)	//Copy constructor
		:BasicPortMessage(msg)
	{}

	virtual Message *clone() const
	{return new BasicOutputMessage(*this);}

	virtual const std::string type() const
	{return "Y" ;}

protected:
	BasicOutputMessage& operator=(const BasicOutputMessage&);	//Assignment operator
	int operator==(const BasicOutputMessage&) const;	//Equality operator

}; /** class BasicOutputMessage **/

class OutputMessage : public BasicOutputMessage
{
public:
	OutputMessage()
	{ userDefined = false; }

	OutputMessage( const VTime &tm, const ProcId &m, const Port &port, const AbstractValue &val )
	:BasicOutputMessage(tm,m, port, BasicMsgValue::build_from(val))//(BasicMsgValue*) new RealMsgValue(val))
	{}

	OutputMessage(const OutputMessage &msg)	//Copy constructor
		:BasicOutputMessage(msg)
	{}

	const value_ptr &value() const
	{return (BasicOutputMessage::value())->v;}

	Message &value( const Value &val )
	{BasicOutputMessage::value((BasicMsgValue*) new RealMsgValue(val)); return *this;}

	virtual Message *clone() const
	{return new OutputMessage(*this);}

protected:
	OutputMessage& operator=(const OutputMessage&);	//Assignment operator
	int operator==(const OutputMessage&) const;	//Equality operator

};	// class OutputMessage



#endif   //__MESSAGE_H 
