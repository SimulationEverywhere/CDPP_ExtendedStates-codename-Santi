/*******************************************************************
*
*  DESCRIPTION: class Processor
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

/** include files **/
#include "process.h"    // header
#include "model.h"      // class Model
#include "msgadm.h"     // SingleMsgAdm 

using namespace std;

/** public data **/
const ProcId Processor::InvalidId( -1 ) ;

/** public functions **/
/*******************************************************************
* Function Name: Processor
********************************************************************/
Processor::Processor( Model *m )
: mdl( m )
, ident( InvalidId ) 
{
	MASSERT( m );
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
Processor &Processor::receive( const InitMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive InitMessage!" ) ;
	MTHROW( e ) ;
	return *this ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
Processor &Processor::receive( const InternalMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive InternalMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
Processor &Processor::receive( const OutputMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive OutputMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
Processor &Processor::receive( const ExternalMessage &)
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive ExternalMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Function Name: receive
* Description: just throw exception
********************************************************************/
Processor &Processor::receive( const DoneMessage & )
{
	InvalidMessageException e ;
	e.addText( "The abstract base class Processor cannot receive DoneMessage!" ) ;
	MTHROW( e ) ;
}

/*******************************************************************
* Method: sendOutput
********************************************************************/
Processor &Processor::sendOutput( const VTime &time, const Port &port, const Value &value )
{
	SingleMsgAdm::Instance().send( OutputMessage( time, id(), port, value ), model().parentId() ) ;
	return *this;
}

Processor::~Processor()
{
	delete mdl;
}

/*******************************************************************
* Function Name: description
********************************************************************/
const string Processor::description() const
{
        return mdl->description();
}
