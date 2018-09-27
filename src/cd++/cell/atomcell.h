/*******************************************************************
*
*  DESCRIPTION: class AtomicCell (Abstract)
*
*  AUTHOR:    Amir Barylko, Jorge Beyoglonian
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 17/09/1999 (v2)
*
*******************************************************************/

#ifndef __ATOMIC_CELL_H
#define __ATOMIC_CELL_H

#include <map>


/** include files **/
#include "portlist.h"         // for the PortList definition
#include "atomic.h"           // base class
#include "neighval.h"         // NeighborhoodValue
#include "ltranadm.h"         // LocalTransAdmin
#include "cellpos.h"          // class CellPos and NeighborPosition
#include "real.h"             // The value of the cell
#include "statevars.h"        // The state variables

/** forward declarations **/
class CoupledCell ;
class ParallelMainSimulator ;

#define	ATOMIC_CELL_NAME		"AtomicCell"

/** declarations **/
class AtomicCellState: public AtomicState
{

public:
	Real			Quantum;
	PortValues		inValues;	// record the last value that arrives in each port
	NeighborhoodValue	neighborhood;
        StateVars		variables;

	AtomicCellState(){};
	virtual ~AtomicCellState(){};

	AtomicCellState& operator=(AtomicCellState& thisState); //Assignment
	void copyState(AtomicCellState *);
	int  getSize() const;

};

class AtomicCell: public Atomic
{
public:
	static const std::string cellClassName;
	static const std::string outPort;
	static const std::string neighborChangePort;
	static const std::string NCInPrefix;
	static const std::string NCOutPrefix;

	~AtomicCell();

	virtual std::string className() const
	{ return ATOMIC_CELL_NAME; }

	const CellPosition& cellPosition() const 
	{ return cellPos; }

	AtomicCell &localFunction( const LocalTransAdmin::Function &lf )
	{ localFn = lf; return *this; }

	bool addInPort( std::string portName );
		// Agrega un port de entrada. Si ya existia devuelve FALSE,
		// sino devuelve TRUE.

	bool addOutPort( std::string portName );
		// Agrega un port de salida. Si ya existia devuelve FALSE,
		// sino devuelve TRUE.

	void createNCPorts( std::list<std::string> &portNames);
		// Crea los puertos NC.

	void getOutPorts(VirtualPortList *vpl);
		// Devuelve una std::lista de los puertos de salida

	const AbstractValue &value(const std::string &port) const
	{ return neighborhood().get(port); }

	AbstractValue &variable(const std::string &name)
        { return variables().get(name); }

	AbstractValue &variable(const std::string &name, AbstractValue &value)
        { return variables().set(name, value); }

	//Port & port( const std::string & );

	//Port & port( const PortId & );

protected:
	friend class CoupledCell;   	  // value
	friend class SendNCPortNode;
	friend class ParallelSCellCoordinator;
	friend class ParallelMCellCoordinator;
	friend class ParallelMainSimulator; // neighborhood( NeighborhoodValue * )
	friend class LocalTransAdmin; // evaluate
	friend class TransportDelayCell; // calculateInPort()

	AtomicCell( const CellPosition& pos, const std::string & = cellClassName, const LocalTransAdmin::Function &id = LocalTransAdmin::InvalidFn) ; 

	Model &outputFunction( const CollectMessage & );

	virtual AtomicCell &localTransitionConfluent( const MessageBag& );

	virtual void initializeCell();

	ModelState* allocateState() 
	{ return new AtomicCellState;}

	const LocalTransAdmin::Function &localFunction() const
	{ return localFn; }

	void setPortInFunction( const std::string portName, const std::string functionName );

	void setPortValue( const std::string portName, const AbstractValue &portValue );

	const NeighborhoodValue &neighborhood() const
	{ return ((AtomicCellState*)getCurrentState())->neighborhood; }

	NeighborhoodValue &neighborhood()
	{ return ((AtomicCellState*)getCurrentState())->neighborhood; }

	// Puertos OUT
	const Port &outputPort(const std::string &portName) const;

	// Puertos NEIGHBORCHANGE
	const Port &neighborPort(const std::string &portName) const;
	
	PortList &inputPort()		// Lista de puertos IN (Dinamica)
	{ return in; }

	PortInFunction &inputPortFunction()
	{ return inFunction; }

	PortValues &inputPortValues()
	{ return ((AtomicCellState*)(getCurrentState()))->inValues; }

	PortList &outPortList()		// Lista de Puertos OUT
	{ return output; }		// (Dinamica)

	PortList &inNCPortList()	// Lista de Puertos IN de NC
	{ return Xports; }

	PortList &outNCPortList()	// Lista de Puertos OUT de NC
	{ return Yports; }
	
	std::list<std::string> &NCPorts()		// Lista de nombres de puertos NC
	{ return NCPortNames; }
	
	bool isInNCPort( const std::string portName ) const
	{ return (getNCPortByName(Xports, portName) != NULL); }

	bool isOutNCPort( const std::string portName ) const
	{ return (getNCPortByName(Yports, portName) != NULL); }

        StateVars &variables()
        { return (dynamic_cast<AtomicCellState*>(getCurrentState()))->variables; }

	AtomicCell &value( const AbstractValue &val, const std::string &port )
	{ neighborhood().set(port, val); return *this; }

	AtomicCell &setAllNCPortsValues( const AbstractValue &val );

        AtomicCell &variables( const StateVars &vars )
        { (dynamic_cast<AtomicCellState*>(getCurrentState()))->variables = vars;
          return *this; }
        
	Real QuantumValue() const;

	void QuantumValue( const Real& );

	std::string calculateInPort(std::string &portName);
	std::string calculateOutPort(std::string &portName);
	
private:
	bool addInputNCPort( std::string portName );
	bool addOutputNCPort( std::string portName );
	const Port *getNCPortByName(const PortList &pl, const std::string portName) const;
	
	LocalTransAdmin::Function localFn ;

	// Atomic model in and out ports
	PortList        Xports;		// In
	PortList        Yports;		// Out
	std::list<std::string>    NCPortNames;    // NC port names

	// Coupled model in and out ports
	PortList 	in;		// record the In Ports (Dynamic)
	PortInFunction  inFunction;	// record the function to use when a message come from an In Port

	PortList 	output;		// record the Out Ports (Dynamic)
	CellPosition	cellPos;
	
} ; // AtomicCell


/** inline **/
inline
Real AtomicCell::QuantumValue() const {
	return ((AtomicCellState*)getCurrentState())->Quantum;
}

inline
void AtomicCell::QuantumValue( const Real & r){
	((AtomicCellState*)getCurrentState())->Quantum = r;	
}
#endif // __ATOMIC_CELL_H

