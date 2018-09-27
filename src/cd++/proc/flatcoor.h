/*******************************************************************
*
*  DESCRIPTION: class FlatCoordinator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/


#ifndef __FLAT_COORDINATOR_H
#define __FLAT_COORDINATOR_H

/** include files **/
#include "coordin.h"    // class Coordinator

/** foward declarations **/
class Port;

/** declarations **/

class FlatCoordinator : public Coordinator
{
public:
	FlatCoordinator( Coupled * );	//constructor

	Processor &receive( const InitMessage & ) ;
	Processor &receive( const InternalMessage & ) ;
	Processor &receive( const OutputMessage & ) ;
	Processor &receive( const ExternalMessage & ) ;
	Processor &receive( const DoneMessage & ) ;

private:
	long	receivedX;
};

/** inline **/

inline
FlatCoordinator::FlatCoordinator( Coupled * c ) : Coordinator( c )
{
	receivedX = 0;
}

#endif // __FLAT_COORDINATOR_H
