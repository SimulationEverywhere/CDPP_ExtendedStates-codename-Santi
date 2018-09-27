/*******************************************************************
*
*  DESCRIPTION: class Simulator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __SIMULATOR_H
#define __SIMULATOR_H

/** include files **/
#include "process.h"       // class Processor

/** forward declarations **/
class Atomic ;

/** declarations **/
class Simulator: public Processor
{
public:
	Simulator( Atomic * );	 // Default constructor

	Processor &receive( const InitMessage     & ) ;
	Processor &receive( const InternalMessage & ) ;
	Processor &receive( const ExternalMessage & ) ;

protected:
	Processor &id( const ProcId &pid ) ;

};	// class Simulator

#endif   //__SIMULATOR_H 
