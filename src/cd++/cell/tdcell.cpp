/*******************************************************************
*
*  DESCRIPTION: class TransportDelayCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Version 3: Gabriel Wainer
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 03/07/1999 (v2)
*  DATE: 06/02/2000
*
*  Suggested changes:
*	21/04/2001 - Alejandro Troccoli.
*		In the output queue, save the output time instead of the delay.
*		This saves processing time because it is no longer necessary to update the remaining time
*		every time a new internal transition occurs.
*
*******************************************************************/

// ** include files **//
#include "tdcell.h"     // header class
#include "message.h"    // class InternalMessage
#include "coupcell.h"	// CoupledCell
#include "realfunc.h"	// calculateWithQuantum
#include "strutil.h"

using namespace std;

/***************************************
 * TRANSPORT DELAY CELL STATE
 ***************************************/
/***************************************
 * Method: operator=
 ***************************************/
TDCellState &TDCellState::operator=(TDCellState& thisState)
{
	(AtomicCellState &)*this = (AtomicCellState &)thisState;
	queueVal = thisState.queueVal;

	return *this;
}
/***************************************
 * Method: copyState
 ***************************************/
void TDCellState::copyState(TDCellState *thisState)
{
	*this = *(TDCellState *) thisState;
}	

/***************************************
 * Method: getSize()
 ***************************************/
int  TDCellState::getSize() const
{
	return sizeof(TDCellState);	
}

/***************************************
 * TRANSPORT DELAY CELL
 ***************************************/
/*******************************************************************
* Method: TransportDelayCell
********************************************************************/
TransportDelayCell::TransportDelayCell( const CellPosition& cellPos, const string &name, const LocalTransAdmin::Function &fn )
: AtomicCell( cellPos, name, fn )
{
}

/*******************************************************************
* Method: initializeCell
********************************************************************/
void TransportDelayCell::initializeCell()
{
	AtomicCell::initializeCell();

	//This function requires the cell state to be already created.
	if  ( UseQuantum().Active() ) 
		QuantumValue( Real(UseQuantum().Value()) );

}

/*******************************************************************
* Method: initFunction
* Description:
********************************************************************/
Model &TransportDelayCell::initFunction()
{
	PortList::iterator cursor;

	//To start the simulation, hold in for 0 time and queue the present value in the output queue list.
	//This will cause all models to evaluate their local transition function at time zero.
	
	for (cursor = outNCPortList().begin(); cursor != outNCPortList().end(); cursor++) {
		string out_port(cursor->second->name());
		string in_port(calculateInPort(out_port));
		
		TDCellState::QueueValue QV(out_port, AbstractValue::to_value_ptr(value(in_port)));

		queueVal().push_back( TDCellState::QueueElement( VTime::Zero, QV ) );
	}
	holdIn(AtomicState::active, VTime::Zero);

	return *this;

}

/*******************************************************************
* Method: externalFunction
********************************************************************/
Model &TransportDelayCell::externalFunction( const MessageBag &msgs )
{


	//This function must be called before processing any messages!
	//It is this function who will set the neighborhood value
	//correctly
	localTransitionConfluent( msgs );

	//This is should be removed once the new Cell Devs definition language is
	//approved and considers multiple messages from external models
	bool executedLocal = false;

	VTime delay( static_cast<const CoupledCell*>( parent() )->defaultDelay() ) ;
	VTime actualTime( msgs.time() );

	list<PortValue> tv;

	//////////////////////////////////////////////////////////////////////
	//Right now, for this implementation, give priority to portInTransitions
	//This should be changed!!

	for ( MessageBag::iterator cursor = msgs.begin(); cursor != msgs.end(); cursor++)
	{

		ExternalMessage msg = *((ExternalMessage *)(*cursor));

		if( !executedLocal && !isInNCPort( msg.port().name() )  ) {

			string functionName = inputPortFunction()[ msg.port().name() ];

			if (functionName == DEFAULT_FUNCTION_InPort) {
				PortValue pv(msg.port().name(), msg.value());
				tv.push_back(pv);
			} else {	// sino es un PortInTransition valido
				VirtualPortList	*vpl = new VirtualPortList;
				getOutPorts(vpl);

				tv = SingleLocalTransAdmin::Instance().evaluate( functionName, neighborhood(), &(inputPortValues()), delay, actualTime, vpl, this, msg.port().name() ) ;

				delete vpl;
			}//if

			executedLocal = true;
		}//if
		else if ( executedLocal && !isInNCPort( msg.port().name() ) )
		{
			cerr << "At: " << msgs.time() << endl;
			cerr << "Warning: Current implementation can not handle multiple messages from external models!!. Some messages have been ignored!" << endl;
		}

	}//for

	//If there were no messages in ports that were not neighbor ports
	if ( !executedLocal ) {

		VirtualPortList	*vpl = new VirtualPortList;
		getOutPorts(vpl);

		tv = SingleLocalTransAdmin::Instance().evaluate( localFunction(), neighborhood(), NULL, delay, actualTime, vpl, this ) ;

		delete vpl;

	}

	///////////////////////////////////////////////////////////////////////

	VTime                     valueTime = actualTime + delay;
	list<PortValue>::iterator pvCursor;

	//cout << "Cell: " << cellPosition().print() << endl;
	//cout << "UseQuantum().Active() " << UseQuantum().Active() << " UseDynQuantum().Active() " << UseDynQuantum().Active() << endl;
	//cout << "tv = " << tv.asString() << " ltb = " << ltb.asString() << endl;
	//cout << "valueWithQuantum(tv, QuantumValue() = " << valueWithQuantum(tv, QuantumValue()).asString() << endl;
	//cout << "valueWithQuantum(ltb, QuantumValue() = " << valueWithQuantum(ltb, QuantumValue()).asString() << endl;


	for (pvCursor = tv.begin(); pvCursor != tv.end(); pvCursor++)
	{
		string in_port = calculateInPort(pvCursor->first);
		const AbstractValue &ltb = value(in_port);
		value_ptr rc = pvCursor->second;

		if( (UseQuantum().Active() || UseDynQuantum().Active())  &&
				( *valueWithQuantum(*rc, QuantumValue()) != *valueWithQuantum(ltb, QuantumValue()) )
				|| ( !UseQuantum().Active()  && (ltb != *rc )  )  )
		{
			TDCellState::Queue::iterator cursorAux;

			//En esta version si ya existe un valor programado para la hora t, se mantiene ese valor y se programa uno
			//nuevo
			for ( cursorAux = queueVal().begin(); cursorAux != queueVal().end() && cursorAux->first <= valueTime; cursorAux++ );

				TDCellState::QueueValue qv(calculateOutPort(pvCursor->first), pvCursor->second);

				queueVal().insert( cursorAux, TDCellState::QueueElement(valueTime, qv) );

			holdIn( AtomicState::active, firstQueuedTime() - msgs.time()  );
		}

		//If using DynamicQuantum, calculate the new Quantum value.
		if ( UseDynQuantum().Active() )
		{
			if ( *valueWithQuantum(*rc, QuantumValue()) == *valueWithQuantum(ltb, QuantumValue()))
			{
				if ( UseDynQuantum().Strat() )
					QuantumValue ( QuantumValue() * (1-UseDynQuantum().Ratio()));
				else
					QuantumValue ( QuantumValue() * (1+UseDynQuantum().Ratio()));
			}

			//GW
			// Esta es una nueva modificacion: si son distintos, lo reduzco.
			else
			{
				if ( !UseDynQuantum().Strat() )
					QuantumValue ( QuantumValue() * (1-UseDynQuantum().Ratio()));
				else
					QuantumValue ( QuantumValue() * (1+UseDynQuantum().Ratio()));
			}

			//cout << " q act = " << QuantumValue().value() << endl;

		}	
	} // for

	return *this ;
}

/*******************************************************************
* Method: internalFunction
********************************************************************/
Model &TransportDelayCell::internalFunction( const InternalMessage &msg )
{
	MASSERT( !queueVal().empty() && firstQueuedTime() == msg.time() ) ;

	//Delete all messages that are scheduled for the current time
	while (!queueVal().empty() && firstQueuedTime() == msg.time() )
		queueVal().erase( queueVal().begin() );

	if( queueVal().empty() )
		passivate() ;
	else
		holdIn( AtomicState::active, ( firstQueuedTime() - msg.time() ) );

	return *this ;
}

/*******************************************************************
* Method: outputFunction
********************************************************************/
Model &TransportDelayCell::outputFunction( const CollectMessage &msg )
{
	//Send all messages that are scheduled for the current time
	TDCellState::Queue::iterator cursor;

	for( cursor = queueVal().begin() ; cursor != queueVal().end() && cursor->first == msg.time() ; cursor++ )
	{
		//sendOutput( msg.time(), outputPort(), cursor->second.value() ) ;
		sendOutput( msg.time(), outputPort(cursor->second.first), *cursor->second.second ) ;
	}

	return *this ;
}

/*******************************************************************
* Function Name: firstQueuedTime
********************************************************************/
const VTime & TransportDelayCell::firstQueuedTime() const
{
	return queueVal().begin()->first;
}


/*******************************************************************
* Function Name: firstQueuedValue
********************************************************************/
const AbstractValue &TransportDelayCell::firstQueuedValue() const
{
	TDCellState::QueueValue &QV = queueVal().begin()->second;

	return *QV.second;
}

/*******************************************************************
* Function Name: firstQueuedPort
********************************************************************/
const string &TransportDelayCell::firstQueuedPort() const
{
	TDCellState::QueueValue &QV = queueVal().begin()->second;

	return QV.first;
}

/*******************************************************************
* Function Name: updateRemainingTime
********************************************************************/
TransportDelayCell & TransportDelayCell::updateRemainingTime( const VTime &elapsed )
{
	for( TDCellState::Queue::iterator cursor = queueVal().begin() ; cursor != queueVal().end() ; cursor++ )
		cursor->first -= elapsed ;

	return *this;
}
