/*******************************************************************
*
*  DESCRIPTION: class Transducer
*               The transducer calculates statistical information 
*               analizing the processing time.  
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*	    Alejandro Troccoli (v2)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 01/02/2001
*
*******************************************************************/

#ifndef __TRANSDUCER_H
#define __TRANSDUCER_H

#include <map>
#include "atomic.h"
#include "atomicstate.h"
#include "message.h"

#define TRANSDUCER_MODEL_NAME "Transducer"

class TransducerState : public AtomicState {

public:

	typedef std::map< int, VTime, std::less<int> > JobsList ;

	long procCount ;
	long cpuLoad ;
	JobsList unsolved; 

	TransducerState(){};
	virtual ~TransducerState(){};

	TransducerState& operator=(TransducerState& thisState); //Assignment
	void copyState(TransducerState *);
	int  getSize() const;

};


class Transducer: public Atomic
{
public:
	Transducer( const std::string &name = "Transducer" ) ;	//Default constructor

	virtual std::string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const CollectMessage & );

	ModelState *allocateState() {
		return new TransducerState;
	}

private:


	const Port &arrived ;
	const Port &solved ;

	Port &throughput ;
	Port &cpuUsage ;   

	VTime frec ;
	VTime tUnit ;

	const VTime &frecuence() const ;
	const VTime &timeUnit() const ;

	//Shortcut to state members
	long procCount() const;
	void procCount( long );

	long cpuLoad() const;
	void cpuLoad(long);

	TransducerState::JobsList &unsolved();

};	// class Transducer

// ** inline ** // 
inline
	std::string Transducer::className() const
{
	return "Transducer" ;
}

inline
	const VTime &Transducer::frecuence() const
{
	return frec ;
}


inline
	const VTime &Transducer::timeUnit() const
{
	return tUnit ;
}

/*******************************************************************
* Shortcuts to state paramters
*********************************************************************/
inline 
long Transducer::procCount() const {
	return ((TransducerState*)getCurrentState())->procCount;
}

inline
void Transducer::procCount( long p) {
	((TransducerState*)getCurrentState())->procCount = p;
}

inline 
long Transducer::cpuLoad() const {
	return ((TransducerState*)getCurrentState())->cpuLoad;
}

inline
void Transducer::cpuLoad( long l) {
	((TransducerState*)getCurrentState())->cpuLoad = l;
}


inline
TransducerState::JobsList &Transducer::unsolved() {
	return ((TransducerState*)getCurrentState())->unsolved;
}

#endif   //__TRANSDUCER_H 
