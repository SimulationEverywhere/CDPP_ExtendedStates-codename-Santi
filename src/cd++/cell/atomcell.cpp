/*******************************************************************
*
*  DESCRIPTION: class AtomicCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 25/04/1999 (v2)
*
*******************************************************************/

// ** include files **
#include "atomcell.h"      // base header
#include "message.h"       // InternalMessage
#include "except.h"        // MASSERTMSG
#include "strutil.h"       // function lowerCase
#include "pmodeladm.h"

using namespace std;

// ** public data **
const string AtomicCell::cellClassName( "AtomicCell" ) ;

const string AtomicCell::outPort( "out" ) ;
const string AtomicCell::neighborChangePort( "neighborChange" ) ;
const string AtomicCell::NCInPrefix( "in_" );
const string AtomicCell::NCOutPrefix( "out_" );


/***************************************
 * ATOMIC CELL STATE
 ***************************************/
AtomicCellState &AtomicCellState::operator=(AtomicCellState& thisState)
{
	(AtomicState &)*this = (AtomicState &)thisState;
	inValues = thisState.inValues;
	Quantum = thisState.Quantum;
	neighborhood = thisState.neighborhood;
	return *this;
}

void AtomicCellState::copyState(AtomicCellState *thisState)
{
	*this = *(AtomicCellState *) thisState;
}	

int  AtomicCellState::getSize() const
{
	return sizeof(AtomicCellState);	
}

/***************************************
 * ATOMIC CELL 
 ***************************************/


void AtomicCell::setPortInFunction( const string portName, const string functionName )
{
	inputPortFunction()[ portName ] = functionName;
}

/* This function is overloaded by those in tdcell.cpp and idcell.cpp */
Model &AtomicCell::outputFunction( const CollectMessage &msg )
{
	sendOutput( msg.time(), outputPort(outPort), this->value(neighborChangePort) );
	return *this;
}

/*******************************************************************
* Function Name: ~AtomicCell
********************************************************************/
AtomicCell::~AtomicCell()
{}

/*******************************************************************
* Method: AtomicCell
* Description: Constructor
********************************************************************/
AtomicCell::AtomicCell( const CellPosition& pos, const string &name, const LocalTransAdmin::Function &lf ) 
: Atomic( name )
	, localFn( lf )
{
	cellPos = pos;
}

/*******************************************************************
* Method: initializeCell
* Description: This function is called once the current processor state
* has been create. The cell can now create it's neighborhood.
********************************************************************/
void AtomicCell::initializeCell()
{
	//Create the neighborhood
	const CoupledCell& coupled = (CoupledCell&)(*parent());
	neighborhood().create( coupled , coupled.neighborhood(), cellPosition(), NCPortNames);

	//Initialize the PortValues list
	PortList::iterator	cursor;

	for (cursor = inputPort().begin() ;cursor != inputPort().end();
	cursor++ )
		inputPortValues()[ cursor->second->name() ] = AbstractValue::to_value_ptr(Real());
}

/*******************************************************************
* Function Name: addInPort
* Description: Adds an input port if not exists.
********************************************************************/
bool AtomicCell::addInPort( string portName )
{
	// See if the port exists
	string	name( lowerCase( portName ) );

	PortList::iterator	cursor;

	// search the port
	for (cursor = inputPort().begin() ;
	cursor != inputPort().end() && cursor->second->name() != name ;
	cursor++ )
	;

	// If not exists
	if (cursor == inputPort().end() )
	{
		Port	*port = &( addInputPort( name ) ); // Call to the Model's method

		// Adds the port in the IN list
		inputPort()[ port->id() ] = port;

		// Now adds the default function in the port function list
		inputPortFunction()[ name ] = DEFAULT_FUNCTION_InPort;

		// The last value arrived to the port will be set to Undefined
		// in afterProcessorInitialize

		return true;
	}
	return false;
}

/*******************************************************************
* Function Name: addOutPort
* Description: Adds an output port if not exists.
********************************************************************/
bool AtomicCell::addOutPort( string portName )
{
	// See if the port exists
	string	name( lowerCase( portName ) );

	if (name == outPort)		// Si es el puerto "OUT"
		return false;

	PortList::iterator	cursor;

	// search the port
	for (cursor = outPortList().begin() ;
	cursor != outPortList().end() && cursor->second->name() != name ;
	cursor++ )
	;

	// If not exists
	if (cursor == outPortList().end() )
	{
		Port	*port = &( addOutputPort( name ) ); // Call to the Model's method

		// Adds the port in the IN list
		outPortList()[ port->id() ] = port;

		return true;
	}
	return false;
}

/*******************************************************************
* Function Name: addInputNCPort
* Description: Adds a NC port if it is not already in the list.
*              The prefix must be part of the portname
********************************************************************/
bool AtomicCell::addInputNCPort( string portName )
{
	string name(lowerCase(portName));
	
	// If it doesn't exists
	if (!isInNCPort(name)) {
		Port &port = addInputPort(name);

		// Adds the port in the list
		inNCPortList()[port.id()] = inputPorts()[port.id()];

		return true;
	}
	return false;
}

/*******************************************************************
* Function Name: addOutputNCPort
* Description: Adds a NC port if it is not already in the list
*              The prefix must be part of the portname
********************************************************************/
bool AtomicCell::addOutputNCPort( string portName)
{
	// See if the port exists
	string name(lowerCase(portName));

	// If it doesn't exists
	if (!isOutNCPort(name)) {
		Port &port = addOutputPort(name);

		// Adds the port in the list
		outNCPortList()[port.id()] = outputPorts()[port.id()];

		return true;
	}
	return false;
}

/*******************************************************************
* Function Name: createNCPorts
* Description: Creates all the in and out NC ports
********************************************************************/
void AtomicCell::createNCPorts( std::list<string> &portNames)
{
	list<string>::iterator cursor;
	
	NCPortNames = portNames;
	
	addInputNCPort(neighborChangePort);
        addOutputNCPort(outPort);

	for (cursor = NCPortNames.begin(); cursor != NCPortNames.end(); cursor++) {
		MASSERTMSG(addInputNCPort(NCInPrefix + *cursor),
		           "NeighborChange input port duplicated: " + *cursor);
		MASSERTMSG(addOutputNCPort(NCOutPrefix + *cursor),
		           "NeighborChange output port duplicated: " + *cursor);
	}
}

/*******************************************************************
* Function Name: setPortValue
* Description: set the last value arrived to a port
********************************************************************/
void AtomicCell::setPortValue( const string portName, const AbstractValue &portValue )
{
	// See if the port exists
	string	name( lowerCase( portName ) );

	PortList::iterator	cursor;

	// search the port
	for (cursor = inputPort().begin() ;
	cursor != inputPort().end() && cursor->second->name() != name ;
	cursor++ )
	;

	// If doesn't exists, then STOP !!
	MASSERTMSG( cursor != inputPort().end(), "The port does not exist, in method AtomicCell::setPortValue(port, value)" );

	// Now set the last value arrived to the port with a undefined value
	inputPortValues()[ name ] = AbstractValue::to_value_ptr(portValue);
}

/*******************************************************************
* Function Name: getOutPort
* Description: gets an output port
********************************************************************/
void AtomicCell::getOutPorts(VirtualPortList *vpl)
{
	PortList::iterator	cursor = outPortList().begin();

	while (cursor != outPortList().end() )
	{
		vpl->insert( VirtualPortList::value_type( cursor->second->name(), cursor->second ) );
		cursor++;
	}
}

/*******************************************************************
* Function Name: localTransitionConfluent
* Description: this functions receives all the external messages
*	and sets the neighborhood accordingly. This is the function
*	that defines the policy for how to handle multiple external
*	messages from a same cell
********************************************************************/
AtomicCell &AtomicCell::localTransitionConfluent( const MessageBag& msgs)
{

	const CoupledCell* coupcell = (CoupledCell*) parent();

	//Default localTransitionConfluent will do the following:
	//Set neighbor's values to the last value received
	//Set all port values
	//Which local transition function to call will be
	//decided in the externalFunction

	for ( MessageBag::iterator cursor = msgs.begin(); cursor != msgs.end();	cursor++)
	{
		ExternalMessage msg = *((ExternalMessage *)(*cursor));

		if (isInNCPort(msg.port().name())) {
			NeighborPosition npos;
			AtomicCell& cell = (AtomicCell &)SingleParallelModelAdm::Instance().model( msg.senderModelId() );
			
			npos = coupcell->inverseNeighborhood( cellPosition(), cell.cellPosition());
			//cout << "Cell " << cellPosition().print() << "received from " << cell.cellPosition().print() << " which is " << npos.print() << " value: " << msg.value() << endl;
			neighborhood().set(npos, msg.port().name(), *msg.value());
		} else 	{	
			// first we set the new port value in the list of PortValues
			setPortValue( msg.port().name(), *msg.value() );
		}

	}//if

	return *this;

}//localTransitionConfluent


/*******************************************************************
* Function Name: inputPort
* Description: Retorna el input port solicitado, por default is neighborChange
********************************************************************/
const Port &AtomicCell::neighborPort(const string &portName = neighborChangePort) const
{
	const Port *port;
	string intName(lowerCase(portName));	
	
	if (portName != neighborChangePort)
		intName = NCInPrefix + intName;
	
	port = getNCPortByName(Xports, intName);
	MASSERTMSG(port != NULL, "Reference to unexisting port " + portName);
	
	return *port;
}

/*******************************************************************
* Function Name: outputPort
* Description: Retorna el output port solicitado, por default is out
********************************************************************/
const Port &AtomicCell::outputPort(const string &portName = outPort) const
{
	const Port *port;
	string intName(lowerCase(portName));
	
	/* If it isn't "out" and the name isn't already prefixed, prefix it */
	if (portName != outPort && intName.find(NCOutPrefix) != 0)
		intName = NCOutPrefix + intName;
	
	port = getNCPortByName(Yports, intName);
	MASSERTMSG(port != NULL, "Reference to unexisting port " + portName);
	
	return *port;
}

/*******************************************************************
* Function Name: getNCPortByName
* Description: Retorns the output port required. Default is out.
*              The prefix must be part of the portname
********************************************************************/
const Port *AtomicCell::getNCPortByName(const PortList &pl, const string portName) const
{
	string                   name(lowerCase(portName));
	PortList::const_iterator cursor = pl.begin();
	
	while (cursor != pl.end() && cursor->second->name() != name)
		cursor++;
		
	return (cursor != pl.end() ? cursor->second : NULL);
}

/*******************************************************************
* Function Name: calculateInPort
* Description: Creates an input port name out of the available port name.
********************************************************************/
string AtomicCell::calculateInPort( string &portName )
{

	/* Check whether it is the port default */
	if (portName == outPort)
		return neighborChangePort;

	/* Check whether it is a port from the coupled model */
	PortList &pl = inputPort();
	PortList::iterator cursor = pl.begin();
	while (cursor != pl.end() && lowerCase(cursor->second->name()) != lowerCase(portName))
		cursor++;

	if (cursor != pl.end())
		return neighborChangePort;
	
	
	/* Check if it is an output NC port */
	string            result(portName);
	string::size_type pos;
	
	pos = result.find(NCOutPrefix);
	if (pos != string::npos)
		result.replace(pos, NCOutPrefix.length(), NCInPrefix);

	return result;
}


/*******************************************************************
* Function Name: calculateOutPort
* Description: Creates an output port name out of the available port name.
********************************************************************/
string AtomicCell::calculateOutPort( string &portName )
{

	/* Check whether it is the port default */
	if (portName == neighborChangePort)
		return outPort;

	/* Check whether it is a port from the coupled model */
	PortList &pl = inputPort();
	PortList::iterator cursor = pl.begin();
	while (cursor != pl.end() && lowerCase(cursor->second->name()) != lowerCase(portName))
		cursor++;

	if (cursor != pl.end())
		return outPort;
	
	
	/* Check if it is an input NC port */
	string            result(portName);
	string::size_type pos;
	
	pos = result.find(NCInPrefix);
	if (pos != string::npos)
		result.replace(pos, NCInPrefix.length(), NCOutPrefix);

	return result;
}


/*******************************************************************
* Function Name: setAllNCPortsValues
* Description: set the same value for all the input NC ports.
********************************************************************/
AtomicCell &AtomicCell::setAllNCPortsValues( const AbstractValue &val )
{
	PortList::iterator cursor;
	
	for (cursor = inNCPortList().begin(); cursor != inNCPortList().end(); cursor++)
		value( val, cursor->second->name() );
		
	return *this;
}



