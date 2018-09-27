/*******************************************************************
*
*  DESCRIPTION: class InertialDelayCell
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 17/9/1999 (v2)
*
*******************************************************************/

#ifndef __INERTIAL_DELAY_ATOMIC_CELL_H
#define __INERTIAL_DELAY_ATOMIC_CELL_H

/** include files **/
#include "atomcell.h"         // base class header
#include "value.h"

/** foward declarations **/
#define	INERTIAL_DELAY_CELL_NAME	"InertialDelayCell"

/** declarations **/
class IDCellState : public AtomicCellState
{
public:

	std::map<std::string, value_ptr> futureValue;
	std::map<std::string, value_ptr> actualValue;

	IDCellState(){};
	virtual ~IDCellState(){};

	IDCellState& operator=(IDCellState& thisState); //Assignment
	void copyState(IDCellState *);
	int  getSize() const;

};

class InertialDelayCell: public AtomicCell
{
public:
	InertialDelayCell( const CellPosition& cellPos, const std::string &name = INERTIAL_DELAY_CELL_NAME , const LocalTransAdmin::Function &fn = LocalTransAdmin::InvalidFn )
	: AtomicCell( cellPos, name, fn )
	{}

	std::string className() const
	{return INERTIAL_DELAY_CELL_NAME;}

protected:

	Model &initFunction();
	Model &internalFunction( const InternalMessage & );
	Model &externalFunction( const MessageBag & ) ;
	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState()
	{ return new IDCellState;}

	void initializeCell();
private:

	value_ptr futureValue( const std::string& ) const;
	void futureValue( const std::string&, const AbstractValue& );

	value_ptr actualValue( const std::string& ) const;
	void actualValue( const std::string&, const AbstractValue& );

} ;  // InertialDelayCell

/** inline **/
inline
value_ptr InertialDelayCell::futureValue(const std::string &port) const
{
	return ((IDCellState*)getCurrentState())->futureValue[port];	
}

inline
	void InertialDelayCell::futureValue (const std::string &port, const AbstractValue& r)
{
	((IDCellState*)getCurrentState())->futureValue[port] = AbstractValue::to_value_ptr(r);
}

inline
	value_ptr InertialDelayCell::actualValue(const std::string &port) const
{
	return ((IDCellState*)getCurrentState())->actualValue[port];	
}

inline
	void InertialDelayCell::actualValue (const std::string &port, const AbstractValue& r)
{
	((IDCellState*)getCurrentState())->actualValue[port] = AbstractValue::to_value_ptr(r);
}



#endif // __INERTIAL_DELAY_ATOMIC_CELL_H
