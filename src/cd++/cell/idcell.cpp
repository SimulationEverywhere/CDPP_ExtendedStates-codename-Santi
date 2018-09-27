/*******************************************************************
*
*  DESCRIPTION: class InertialDelayCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*  Version 3: Gabriel Wainer
*  Version 4: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*         mailto://gabrielw@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 17/9/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include "idcell.h"	// header class
#include "message.h"	// ExternalMessage
#include "coupcell.h"	// CoupledCell
#include "realfunc.h"	// calculateWithQuantum

using namespace std;

/***************************************
 * INERTIAL DELAY CELL STATE
 ***************************************/
/***************************************
 * Method: operator=
 ***************************************/
IDCellState &IDCellState::operator=(IDCellState& thisState)
{
	(AtomicCellState &)*this = (AtomicCellState &)thisState;
	actualValue = thisState.actualValue;
	futureValue = thisState.futureValue;


	return *this;
}
/***************************************
 * Method: copyState
 ***************************************/
void IDCellState::copyState(IDCellState *thisState)
{
	*this = *(IDCellState *) thisState;
}	

/***************************************
 * Method: getSize()
 ***************************************/
int  IDCellState::getSize() const
{
	return sizeof(IDCellState);	
}

/***************************************
 * INERTIAL DELAY CELL 
 ***************************************/
/*******************************************************************
* Method: initializeCell
********************************************************************/
void InertialDelayCell::initializeCell()
{
	AtomicCell::initializeCell();

	//This function requires the cell state to be already created.
	if  ( UseQuantum().Active() ) 
		QuantumValue( Real(UseQuantum().Value()) );

}
/*******************************************************************
* Method: externalFunction
********************************************************************/
Model &InertialDelayCell::externalFunction( const MessageBag &msgs )
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

	for ( MessageBag::iterator cursor = msgs.begin(); cursor != msgs.end();
			cursor++)
	{

		ExternalMessage msg = *((ExternalMessage *)(*cursor));

		if (!isInNCPort(msg.port().name())) {

			MASSERTMSG( executedLocal == false, "Current implementation can not handle multiple messages from external models!!");

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
	}//for

	if ( !executedLocal ) {

		VirtualPortList	*vpl = new VirtualPortList;
		getOutPorts(vpl);

		tv = SingleLocalTransAdmin::Instance().evaluate( localFunction(), neighborhood(), NULL, delay, actualTime, vpl, this ) ;

		delete vpl;

	}

	//cout << " Quantum : " << QuantumValue().value() << endl;
	//cout << " ActualValue : " << actualValue().value() << " with q " << valueWithQuantum(actualValue(), QuantumValue()) << endl;
	//cout << " New value : " << tv << " with q " << valueWithQuantum(tv, QuantumValue()) << endl;

	list<PortValue>::iterator pvCursor;
	
	for (pvCursor = tv.begin(); pvCursor != tv.end(); pvCursor++)
	{
		value_ptr rc = pvCursor->second;
		value_ptr rf = actualValue(pvCursor->first);

		if( (UseQuantum().Active() || UseDynQuantum().Active())  &&
				(*valueWithQuantum(*rc, QuantumValue()) != *valueWithQuantum(*rf, QuantumValue()) )
				|| ( !UseQuantum().Active()  && (rf == nullptr || *actualValue(pvCursor->first) != *pvCursor->second )  )  )

		{
			actualValue ( pvCursor->first, *pvCursor->second );	/* El nuevo estado es el recien calculado */
			value_ptr future = futureValue(pvCursor->first);

			if( state() == AtomicState::passive )
				holdIn( AtomicState::active, delay ) ;
			else
				// the coupled sets sigma to the reminder time until the next internal transition
				if( VTime::Zero < nextChange() && (future == nullptr || *future != *rc))
					holdIn( AtomicState::active, delay ) ;
			futureValue( pvCursor->first, *pvCursor->second);
		}

		//If using DynamicQuantum, calculate the new Quantum value.
		if ( UseDynQuantum().Active() )
		{
			value_ptr actual = actualValue(pvCursor->first);
			if ( *valueWithQuantum(*rc,QuantumValue()) == *valueWithQuantum(*actual, QuantumValue()))
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
* Method: initFunction
* Description:
********************************************************************/
Model &InertialDelayCell::initFunction()
{
	PortList::iterator cursor;

	//To start the simulation, send an output message with the current
	//value and passivate. Model will become active on processing the
	//outputs from the other cells.
	
	for (cursor = outNCPortList().begin(); cursor != outNCPortList().end(); cursor++) {
		string out_port(cursor->second->name());
		string in_port(calculateInPort(out_port));
		
		actualValue( out_port, value(in_port) );
		futureValue( out_port, value(in_port) );
	}
	holdIn( AtomicState::active, VTime::Zero);

	return *this;

}

/*******************************************************************
* Method: outputFunction
********************************************************************/
Model &InertialDelayCell::outputFunction( const CollectMessage &msg )
{
	PortList::iterator cursor;

	for (cursor = outNCPortList().begin(); cursor != outNCPortList().end(); cursor++) {
		string out_port(cursor->second->name());
		value_ptr value = actualValue(out_port);

		if(value != nullptr)
			sendOutput( msg.time(), outputPort(out_port), *value );
		else
			sendOutput( msg.time(), outputPort(out_port), Real() );
	}
	return *this;
}

/*******************************************************************
* Method: internalFunction
********************************************************************/
Model &InertialDelayCell::internalFunction( const InternalMessage &msg )
{ 

	passivate(); 
	return *this; 
}

