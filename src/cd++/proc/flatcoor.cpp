/*******************************************************************
*
*  DESCRIPTION: class FlatCoordinator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez. 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 11/04/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include "flatcoor.h"   //headers
#include "flatcoup.h"   // class FlatCoupledCell
#include "msgadm.h"     // SingleMessageAdm
#include "cellstate.h"

using namespace std;

/*******************************************************************
* Method: receive
* Description: InitMessage
********************************************************************/
Processor &FlatCoordinator::receive( const InitMessage &msg )
{
	FlatCoupledCell &coupled( static_cast< FlatCoupledCell & >( model() ) ) ;
	coupled.initFunction();

	lastChange( msg.time() ) ;

	if( coupled.nextEventList().empty() )
		nextChange( VTime::Inf );
	else
		nextChange( coupled.nextEventList().front().time );

	DoneMessage doneMsg( msg.time(), this->Processor::id(), coupled.nextChange(), false ) ;
	SingleMsgAdm::Instance().send( doneMsg, coupled.parentId() ) ;

	return *this;
}

/*******************************************************************
* Method: receive
* Description: ExternalMessage
********************************************************************/
Processor &FlatCoordinator::receive( const ExternalMessage &msg ) 
{
	FlatCoupledCell &coupled( static_cast< FlatCoupledCell & >( model() ) );

	VirtualPortList *xList( coupled.externalList() );
	CellPosition	counter( coupled.dimension().dimension(), 0 );
	register bool	overflow = false;

	receivedX++;

	while (!overflow){
		 for (VirtualPortList::iterator cursor = xList[ counter.calculateIndex( coupled.dimension() )].begin(); cursor != xList[ counter.calculateIndex( coupled.dimension() )  ].end(); cursor++)
			if ( *(cursor->second) == msg.port() )
				coupled.externalFunction( msg.time(), *(const CellPosition *) &counter, true, msg.value(), cursor->first );

		overflow = counter.next( coupled.dimension() );
	}

	lastChange( msg.time() ) ;

	if( coupled.nextEventList().empty() )
		nextChange( VTime::Inf );
	else
		nextChange( coupled.nextEventList().front().time - lastChange() );

	DoneMessage doneMsg( msg.time(), this->Processor::id(), coupled.nextChange(), false ) ;

	for (long i = 1; i <= receivedX; i++)
		SingleMsgAdm::Instance().send( doneMsg, coupled.parentId() );

	receivedX = 0;

	if ( FlatDebug().Active() )
	{
		FlatDebug().Stream() << "VTime: " << msg.time() << endl;
		coupled.cellState()->print( FlatDebug().Stream() ) ;
	}
	return *this;
}

/*******************************************************************
* Method: receive
* Description: IneternalMessage
********************************************************************/
Processor &FlatCoordinator::receive( const InternalMessage &msg ) 
{
	FlatCoupledCell &coupled( static_cast< FlatCoupledCell & >( model() ) ) ;

	coupled.internalFunction( msg.time() );

	lastChange( msg.time() ) ;

	if( coupled.nextEventList().empty() )
		nextChange( VTime::Inf );
	else
		nextChange( coupled.nextEventList().front().time - lastChange() );

	DoneMessage doneMsg( msg.time(), this->Processor::id(), coupled.nextChange(), false ) ;
	SingleMsgAdm::Instance().send( doneMsg, coupled.parentId() ) ;

	if ( FlatDebug().Active() )
	{
		FlatDebug().Stream() << "VTime: " << msg.time() << endl;
		coupled.cellState()->print( FlatDebug().Stream() ) ;
	}
	return *this;
}

/*******************************************************************
* Method: receive
* Description: OutputMessage
********************************************************************/
Processor &FlatCoordinator::receive( const OutputMessage & )
{
	MASSERT( false );
	return *this;
}

/*******************************************************************
* Method: receive
********************************************************************/
Processor &FlatCoordinator::receive( const DoneMessage & ) 
{
	MASSERT( false );
	return *this;
}
