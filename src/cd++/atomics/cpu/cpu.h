/*******************************************************************
*
*  DESCRIPTION: class CPU (processes jobs with a specified
*               distribution)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*	v2: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 01/02/2001
*
*******************************************************************/

#ifndef __CPU_H
#define __CPU_H

/** include files **/
#include "atomic.h"  // class Atomic
#include  "atomicstate.h"

#define CPU_MODEL_NAME "CPU"

/** forward declarations **/
class Distribution ;

class CPUState : public AtomicState {

public:

	//Process id
	int pid;

	CPUState(){};
	virtual ~CPUState(){};

	CPUState& operator=(CPUState& thisState); //Assignment
	void copyState(CPUState *);
	int  getSize() const;

};


/** declarations **/
class CPU: public Atomic
{
public:
	CPU( const std::string &name = "CPU" ) ;	 // Default constructor

	~CPU();					// Destructor

	virtual std::string className() const
	{return "CPU";}

protected:
	Model &initFunction()
	{return *this;}

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

	ModelState* allocateState() {
		return new CPUState;
	}

private:
	const Port &in ;
	Port &out ;
	Distribution *dist ;

	//Shortcuts to cpu's state
	int pid() const;
	void pid( int );

	Distribution &distribution()
	{return *dist;}
};	// class CPU


/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
int CPU::pid() const {
	return ((CPUState*)getCurrentState())->pid;
}

inline
void CPU::pid( int p) {
	((CPUState*)getCurrentState())->pid = p;
}

#endif   //__CPU_H 
