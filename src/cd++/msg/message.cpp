/*******************************************************************
*
*  DESCRIPTION: definitions of messages ( Y, *, D,@, X, I )
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

/** include files **/
#include "message.h"		// base header class
#include "value.h"

using namespace std;

/*******************************************************************
*CLASS BASICMSGVALUE
********************************************************************/	
BasicMsgValue::BasicMsgValue() : v(nullptr) { };
BasicMsgValue::BasicMsgValue(const BasicMsgValue& thisValue) : BasicMsgValue(*thisValue.v)
{}
/*******************************************************************
* Function Name: Destructor
********************************************************************/	
BasicMsgValue::~BasicMsgValue(){}

/*******************************************************************
* Function Name: valueSize
********************************************************************/	
int BasicMsgValue::valueSize() const
{
	return sizeof(BasicMsgValue);
}

/*******************************************************************
* Function Name: asString
********************************************************************/	
string BasicMsgValue::asString() const
{ 
	return string(""); 
}

/*******************************************************************
* Function Name: clone
********************************************************************/	
/*BasicMsgValue* BasicMsgValue::clone() const
{ 
	return new BasicMsgValue;
}*/

BasicMsgValue *BasicMsgValue::build_from(const AbstractValue &val)
{
	BasicMsgValue *ptr;
	AbstractValue *cval = const_cast<AbstractValue*>(&val);

	switch(val.type())
	{
		case CellValueType::real:
		{
			Real *r = dynamic_cast<Real*>(cval);
			ptr = new RealMsgValue(r->value());
			break;
		}
		case CellValueType::tuple:
		{
			Tuple<Real> *t = dynamic_cast<Tuple<Real>*>(cval);
			ptr = new TupleMsgValue<Real>(*t);
			break;
		}
		default:
		{
			MException e;
			MTHROW(e);
			break;
		}
	}

	return ptr;
}

/*******************************************************************
*CLASS REALMSGVALUE
********************************************************************/	
/*******************************************************************
* Function Name: Constructor
********************************************************************/	
RealMsgValue::RealMsgValue() : BasicMsgValue() {};
RealMsgValue::RealMsgValue( const Value& val) : BasicMsgValue(Real(val)){}

/*******************************************************************
* Function Name: Copy Constructor
********************************************************************/	
RealMsgValue::RealMsgValue( const RealMsgValue& thisValue )
{
	v = thisValue.v;
}

/*******************************************************************
* Function Name: valueSize
********************************************************************/	
int RealMsgValue::valueSize() const 
{ 
	return sizeof(RealMsgValue);
}
/*******************************************************************
* Function Name: asString
********************************************************************/	
string RealMsgValue::asString() const 
{ 
	const real_ptr r = dynamic_pointer_cast<Real>(this->v);
	return Value2StrReal(r->value());
}

/*******************************************************************
* Function Name: clone
********************************************************************/	
BasicMsgValue* RealMsgValue::clone() const
{ 
	return new RealMsgValue(*this);
}


/*******************************************************************
* CLASS: Message
********************************************************************/	
/*******************************************************************
* Function Name: asString
********************************************************************/	
const string Message::asString() const {

	Model& mdl = SingleParallelProcessorAdmin::Instance().model( procId());

	return type() + " / " + time().asString() + " / " + mdl.description() + "(" +  procId() + ")";
}


/*******************************************************************
* Function Name: asStringReceived
********************************************************************/	
const string Message::asStringReceived() const {

	string origin;

	Model& mdl = SingleParallelProcessorAdmin::Instance().model( procId());

	if ( mdl.localProc() == proc )
		origin = "L";
	else
		origin = "R";

	return int2Str ( ParallelMainSimulator::Instance().getMachineID() ) + " / " + origin + " / " + asString();

}

/*******************************************************************
* Function Name: asStringSent
********************************************************************/	
const string Message::asStringSent( const ProcId& dest ) const{

	string destination;

	Model& mdl = SingleParallelProcessorAdmin::Instance().model( dest );

	if ( mdl.localProc() == dest )
		destination = "L";
	else
		destination = "R";

	return " << " + int2Str ( ParallelMainSimulator::Instance().getMachineID() ) + " / " + destination + " / " + asString() +  " / " + mdl.description() + "(" + dest + ")";

}

/*******************************************************************
* CLASS: DoneMessage
********************************************************************/	
/*******************************************************************
* Function Name: asString
********************************************************************/	

const string DoneMessage::asString() const 
{
	return Message::asString() + " / " + nextChange().asString(); 
}

/*******************************************************************
* CLASS: BasicPortMessage
********************************************************************/	
/*******************************************************************
* Function Name: asString
********************************************************************/	


const string BasicPortMessage::asString() const
{ 
	return Message::asString() + " / " + port().asString() + " / " + value()->asString(); 
}
