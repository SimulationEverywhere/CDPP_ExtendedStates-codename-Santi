/*******************************************************************
*
*  DESCRIPTION: class Processor ( Base class for Coordinator, 
*                                 Simulator and RootCoordinator )
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __PROCESSOR_H
#define __PROCESSOR_H

/** include files **/
#include "modelid.h"          // class ModelId
#include "VTime.h"             // class Timre
#include "except.h"
#include "value.h"

/** forward declarations **/
class Model ;
class InitMessage ;      
class InternalMessage ;  
class OutputMessage ;    
class ExternalMessage ;   
class DoneMessage ;       
class ProcessorAdmin ;
class Port ;

/** declarations **/
typedef ModelId ProcId ;


class Processor
{
public:
	virtual ~Processor();	 // Destructor
	
	// all the posible messages
	virtual Processor &receive( const InitMessage & ) ;
	virtual Processor &receive( const InternalMessage & ) ;
	virtual Processor &receive( const OutputMessage & ) ;
	virtual Processor &receive( const ExternalMessage &) ;
	virtual Processor &receive( const DoneMessage &) ;
	
	// the Processor Id 
	const ProcId &id() const
	{ return ident;}
	
	// return the asociate model
	Model &model() ;
	const Model &model() const ;
	
	const VTime &nextChange() const ;
	const VTime &lastChange() const ;
	
	const std::string asString() const ;
	
	virtual const std::string description() const;
	
	static const ProcId InvalidId ;
	
protected:
	Processor() ;	 // Default constructor
	Processor( Model * ) ;   
	Processor( const Processor & );	//Copy constructor
	Processor &operator =( const Processor & ) ;	  // Assignment operator
	int operator ==( const Processor & ) const ;	  // Equality operator
	
	friend class Model ;
	Processor &nextChange( const VTime & ) ;
	Processor &lastChange( const VTime & ) ;
	
	virtual Processor &sendOutput( const VTime &, const Port &, const Value & );
	
	friend class StandAloneProcessorAdmin ;
	friend class ParallelProcessorAdmin ; 
	
	virtual Processor &id( const ProcId & ) ;
	
	Model *pmodel() ;
	
private:
	Model *mdl ;
	VTime next ;
	VTime last ; 
	ProcId ident ;
	
};	// class Processor

/** inline **/
inline
	Processor::Processor() 
	: mdl( NULL )
{}

inline
	Model &Processor::model()
{
	return *mdl ;
}

inline
	const Model &Processor::model() const
{
	MASSERT( mdl ) ;
	return *mdl ;
}

inline
	const VTime &Processor::nextChange() const
{
	return next ;
}

inline 
	const VTime &Processor::lastChange() const
{
	return last ;
}

inline 
	Processor &Processor::nextChange( const VTime &t )
{
	next = t ;
	return *this ;
}

inline
	Processor &Processor::lastChange( const VTime &t )
{
	last = t ;
	return *this ;
}

inline
	const std::string Processor::asString() const
{
	return description() + "(" + id() + ")" ;
}


inline
	Processor &Processor::id( const ProcId &id )
{
	ident = id ;
	return *this ;
}

inline
	Model *Processor::pmodel()
{
	return mdl ;
}

#endif   //__PROCESSOR_H 
