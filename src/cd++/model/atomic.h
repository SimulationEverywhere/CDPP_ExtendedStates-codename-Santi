/*******************************************************************
*
*  DESCRIPTION: class Atomic
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  VERSION 2: Daniel Rodriguez
*  VERSION 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 25/4/1999 (v2)
*  DATE: 07/11/2000 (v3)
*
*******************************************************************/

#ifndef __ATOMIC_H
#define __ATOMIC_H

/** include files **/
#include "model.h"   //class Model
#include "atomicstate.h" //class AtomicState
#include "pSimulatorState.h"
#include "message.h"
#include "msgbag.h"


#define	ATOMIC_NAME		"Atomic"

/** definitions **/
class Atomic : public Model
{
public:
	virtual ~Atomic();	// Destructor


protected:
	friend class ParallelSimulator ;

	Atomic( const std::string &name = "Atomic" );	// Constructor

	Atomic( const Atomic & ) ;	 	// Copy constructor

	Atomic &operator = ( const Atomic & );	// Assignment operator

	int operator ==( const Atomic & ) const; // Equality operator

	virtual Model &initFunction() = 0;

	virtual Model &externalFunction ( const MessageBag & );

	virtual Model &externalFunction( const ExternalMessage & );

	virtual Model &internalFunction( const InternalMessage & ) = 0 ;

	virtual Model &outputFunction( const CollectMessage & ) = 0 ;

	virtual Model &confluentFunction ( const InternalMessage &, const MessageBag & );


	Model &holdIn( const AtomicState::State &, const VTime & ) ;

	Model &passivate();


	Model &state( const AtomicState::State &s )
	{ ((AtomicState *)getCurrentState())->st = s; return *this; }

	const AtomicState::State &state() const
	{return ((AtomicState *)getCurrentState())->st;}

	virtual std::string className() const
	{return ATOMIC_NAME;}

	//State functions
	virtual ModelState* allocateState();

	//Returns the current model state. 
	//This method will only be available for those atomic models
	//thar are local to the current machine
	virtual ModelState* getCurrentState() const;

	virtual ModelState* getCurrentState() ;

	virtual ParallelProcessor &createParallelProcessor();

	unsigned long totalProcCount() const;

	unsigned long localProcCount() const;

	double get_optional_double_param(const std::string &, double =0);
	double get_mandatory_double_param(const std::string &);

private:
	friend class MainSimulator ;
	friend class StandAloneMainSimulator;
	friend class ParallelMainSimulator;


};	// class Atomic

#endif   //__ATOMIC_H
