/*******************************************************************
*
*  DESCRIPTION: class TransportDelayCell
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*  Version 2: Daniel Rodriguez
*  Version 3: Gabriel Wainer
*  Version 4: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 02/03/1999 (v2)
*  DATE: 06/02/2000
*  DATE: 21/02/2001
*******************************************************************/

#ifndef __TRANSPORT_DELAY_ATOMIC_CELL_H
#define __TRANSPORT_DELAY_ATOMIC_CELL_H

/** include files **/
#include <list>
#include <utility>
#include "atomcell.h"         // base class header
#include "value.h"

/** foward declarations **/

#define	TRANSPORT_DELAY_CELL_NAME	"TransportDelayCell"

/** declarations **/
class TDCellState : public AtomicCellState 
{
public:
	typedef pair< std::string, value_ptr >      QueueValue ;
	typedef pair< VTime, QueueValue > QueueElement ;
	typedef std::list< QueueElement >      Queue ;

	Queue queueVal ;

	TDCellState(){};
	virtual ~TDCellState(){};

	TDCellState& operator=(TDCellState& thisState); //Assignment
	void copyState(TDCellState *);
	int  getSize() const;

};

class TransportDelayCell: public AtomicCell
{
public:
	TransportDelayCell( const CellPosition& cellPos, const std::string & = TRANSPORT_DELAY_CELL_NAME , const LocalTransAdmin::Function &fn = LocalTransAdmin::InvalidFn ) ;

	std::string className() const;

protected:
	Model &initFunction();
	Model &internalFunction( const InternalMessage & ) ;
	Model &externalFunction( const MessageBag & ) ;
	Model &outputFunction( const CollectMessage & ) ;

	ModelState* allocateState()
	{ return new TDCellState;}

	void initializeCell();

private:

	//Real Quantum; // GW //AT: Quantum is now part of AtomicCellState
	//const Real & QuantumValue() const;  // GW //AT: This is part of AtomicCell

	TDCellState::Queue &queueVal() const;

	const VTime & firstQueuedTime() const;
	const AbstractValue & firstQueuedValue() const;
	const std::string & firstQueuedPort() const;

	TransportDelayCell & updateRemainingTime( const VTime & );
	TransportDelayCell & insertByTime( const VTime &, const std::string &, const Real & );

} ;  // TransportDelayCell

/** inline **/

inline
	std::string TransportDelayCell::className() const
{
	return TRANSPORT_DELAY_CELL_NAME;
}

inline
TDCellState::Queue &TransportDelayCell::queueVal() const {
	return ((TDCellState *) getCurrentState())->queueVal;
}

#endif // __TRANSPORT_DELAY_ATOMIC_CELL_H
