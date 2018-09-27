/*******************************************************************
*
*  DESCRIPTION: class StandAloneModelAdmin
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 27/9/2000
*
*******************************************************************/

/** include files **/
#include "smodeladm.h"         // base header
#include "procadm.h"          // processor header
#include "strutil.h"          // lowerCase 
#include "idcell.h"           // InertialDelayCell
#include "tdcell.h"           // TransportDelayCell
#include "flatcoup.h"         // class FlatCoupledCell

using namespace std;

/** public functions **/

/*******************************************************************
* Function Name: StandAloneModelAdmin
********************************************************************/
StandAloneModelAdmin::StandAloneModelAdmin()
{}

/*******************************************************************
* Function Name: newAtomic
* Description: make a new atomic model and store it in the bd
********************************************************************/
Atomic &StandAloneModelAdmin::newAtomic( const AtomicType &type, const string &modelName )
{
	AtomicKinds::iterator cursor( this->kinds.find( type ) ) ;

	if( cursor == this->kinds.end() )
	{
		InvalidAtomicTypeException e;
		e.addText( string( "The number " ) + type + " is an unregistered type" ) ;
		MTHROW( e ) ;
	}

	NewFunction *fn = cursor->second ;
	Atomic *atomic = (*fn)( modelName ) ;

	SingleProcessorAdmin::Instance().generateProcessor( atomic ) ;
	return *atomic ;
}

/*******************************************************************
* Function Name: newAtomic
* Description: search the AtomicType and call to the another newAtomic method
********************************************************************/
Atomic &StandAloneModelAdmin::newAtomic( const string &name, const string &modelName )
{
	AtomicTypes::iterator cursor( this->types.find( lowerCase( name ) ) ) ;

	if( cursor == this->types.end() )
	{
		InvalidAtomicTypeException e ;
		e.addText( name + " is an unregistered type!" ) ;
		MTHROW( e ) ;
	}
	return this->newAtomic( cursor->second, modelName ) ;
}

/*******************************************************************
* Function Name: newAtomicCell
********************************************************************/
AtomicCell &StandAloneModelAdmin::newAtomicCell( bool inertial, const string &modelName )
{
	AtomicCell *atomicCell ;

	if( inertial )
		atomicCell = new InertialDelayCell( modelName ) ;
	else
		atomicCell = new TransportDelayCell( modelName ) ;

	SingleProcessorAdmin::Instance().generateProcessor( atomicCell ) ;
	return *atomicCell ;
}

/*******************************************************************
* Function Name: newCoupled
* Description: make a new coupled model and store it in the bd
********************************************************************/
Coupled &StandAloneModelAdmin::newCoupled( const string &modelName )
{
	Coupled *coupled = new Coupled( modelName ) ;

	SingleProcessorAdmin::Instance().generateProcessor( coupled ) ;

	return *coupled ;
}

/*******************************************************************
* Function Name: newCoupledCell
********************************************************************/
CoupledCell &StandAloneModelAdmin::newCoupledCell( const string &modelName )
{
	CoupledCell *coupled = new CoupledCell( modelName ) ;

	SingleProcessorAdmin::Instance().generateProcessor( coupled ) ;

	return *coupled ;
}

/*******************************************************************
* Function Name: newFlatCoupledCell
********************************************************************/
FlatCoupledCell &StandAloneModelAdmin::newFlatCoupledCell( const string &modelName )
{
	FlatCoupledCell *coupled = new FlatCoupledCell( modelName ) ;

	SingleProcessorAdmin::Instance().generateProcessor( coupled ) ;

	return *coupled ;
}
