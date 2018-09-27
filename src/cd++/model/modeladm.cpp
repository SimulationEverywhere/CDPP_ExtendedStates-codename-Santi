/*******************************************************************
*
*  DESCRIPTION: class ModelAdmin, class SingleParallelModelAdm
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*	   Alejandro Troccoli (v3)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*	 27/9/2000 (v3)
*
*******************************************************************/

/** include files **/
#include "pmodeladm.h"         // base header
#include "pmodeladm.h"	
#include "smodeladm.h"	
#include "procadm.h"          // processor header
#include "strutil.h"          // lowerCase 
#include "idcell.h"           // InertialDelayCell
#include "tdcell.h"           // TransportDelayCell
#include "flatcoup.h"         // class FlatCoupledCell

using namespace std;

/** private data **/
ModelAdmin *SingleParallelModelAdm::instance( NULL ) ;

/** public functions **/

/*******************************************************************
* Function Name: ModelAdmin
********************************************************************/
ModelAdmin::ModelAdmin()
: typeCount( 0 )
{}

/*******************************************************************
* Function Name: CreateParallel
********************************************************************/

ModelAdmin &SingleParallelModelAdm::CreateParallel()
{
	if( !instance ) {
	
		instance = new ParallelModelAdmin();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: CreateStandAlone
********************************************************************/

ModelAdmin &SingleParallelModelAdm::CreateStandAlone()
{
	if( !instance ) {
		instance = new StandAloneModelAdmin();        	
	}
	return *instance;
}


/*******************************************************************
* Function Name: registerAtomic
* Description: register a new atomic type
********************************************************************/
ModelAdmin::AtomicType ModelAdmin::registerAtomic( const NewFunction &f, const string &description )
{
	this->kinds[ this->typeCount ] = f.duplicate() ;
	this->types[ lowerCase( description ) ] = this->typeCount ;
	this->typeCount++ ;
	return this->typeCount ;
}

/*******************************************************************
* Function Name: add2DB
********************************************************************/
ModelAdmin &ModelAdmin::add2DB( Model * model ) {

	modelDb[model->description()] = model;
	return *this;

}


/*******************************************************************
* Function Name: model
********************************************************************/
Model& ModelAdmin::model( const string modelName ) {

	ModelDB::iterator cursor( modelDb.begin() );

	while( cursor != modelDb.end() && cursor->second->description() != modelName )
		cursor ++ ;

	if( cursor == modelDb.end() )
	{
		InvalidModelIdException e ;
		e.addText( string( "The model " ) + lowerCase( modelName ) + " has not yet been registered!" );
		MTHROW( e ) ;
	}
	
	return *( cursor->second );
}


/*******************************************************************
* Function Name: models
********************************************************************/
const ModelAdmin::ModelDB &ModelAdmin::models() const{
	return modelDb;
}
