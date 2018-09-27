/*******************************************************************
*
*  DESCRIPTION: class ParallelModelAdmin
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 27/9/2000
*
*******************************************************************/

/** include files **/
#include <set>
#include "pmodeladm.h"         // base header
#include "parsimu.h"		   //ParallelMainSimulator
#include "except.h"		   //InvalidModelId exception
#include "strutil.h"          // lowerCase 
#include "prootmodel.h"	  // RootModel
#include "idcell.h"           // InertialDelayCell
#include "tdcell.h"           // TransportDelayCell
#include "flatcoup.h"         // class FlatCoupledCell

using namespace std;

/** private data **/
ParallelModelAdmin *SingleParallelModelAdm::instance( NULL ) ;

/** public functions **/

/*******************************************************************
* Function Name: Constructor
********************************************************************/
ParallelModelAdmin::ParallelModelAdmin()
	: typeCount( 0 ),
modelCount ( 0 )
{}

/*******************************************************************
* Function Name: Destructor
********************************************************************/
ParallelModelAdmin::~ParallelModelAdmin(){

	ModelDB::const_iterator cursor;

	for (cursor = modelDb.begin(); cursor != modelDb.end(); cursor++){

#ifndef USE_VECTOR		
		delete cursor->second;
#else
		delete (*cursor);
#endif
	}

}

/*******************************************************************
* Function Name: registerAtomic
* Description: register a new atomic type
********************************************************************/
ParallelModelAdmin::AtomicType ParallelModelAdmin::registerAtomic( const NewFunction &f, const string &description )
{
	this->kinds[ this->typeCount ] = f.duplicate() ;
	this->types[ lowerCase( description ) ] = this->typeCount ;
	this->typeCount++ ;
	return this->typeCount ;
}

/*******************************************************************
* Function Name: add2DB
********************************************************************/
ParallelModelAdmin &ParallelModelAdmin::add2DB( Model * model ) {

	model->id( newModelId());
	modelDb[model->id()] = model;	
	modelDic[model->description()] = model;
	return *this;

}


/*******************************************************************
* Function Name: model
********************************************************************/
Model& ParallelModelAdmin::model( const string& modelName ) const {

	ModelDic::const_iterator cursor;


	cursor = modelDic.find( modelName );

	if( cursor == modelDic.end() )
	{
		InvalidModelIdException e ;
		e.addText( string( "The model " ) + lowerCase( modelName ) + " has not yet been registered!" );
		MTHROW( e ) ;
	}

	return *( cursor->second );

}

/*******************************************************************
* Function Name: model
********************************************************************/
Model& ParallelModelAdmin::model( const ModelId model ) const {

#ifndef USE_VECTOR
	ModelDB::const_iterator cursor;

	cursor = modelDb.find( model);
	if( cursor == modelDb.end() )
	{
		InvalidModelIdException e ;
		e.addText( string( "The model with id " ) + model + " has not yet been registered!" );
		MTHROW( e ) ;
	}

	return *( cursor->second );
#else

	return *( modelDb[model] );
#endif
}

/*******************************************************************
* Function Name: models
********************************************************************/
const ParallelModelAdmin::ModelDB &ParallelModelAdmin::models() const{
	return modelDb;
}

/*******************************************************************
* Function Name: newRoot
* Description: creates a root model. (Dummy model used to map the root ProcId
* to a model)
********************************************************************/
Model &ParallelModelAdmin::newRoot(){
	Model * root = new RootModel("ParallelRoot");
	add2DB (root);
	return *root;
}

/*******************************************************************
* Function Name: newAtomic
* Description: make a new atomic model and store it in the bd
********************************************************************/
Atomic &ParallelModelAdmin::newAtomic( const AtomicType &type, const string &modelName )
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

	add2DB(atomic);

	return *atomic ;
}

/*******************************************************************
* Function Name: newAtomic
* Description: search the AtomicType and call to the another newAtomic method
********************************************************************/
Atomic &ParallelModelAdmin::newAtomic( const string &name, const string &modelName )
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
AtomicCell &ParallelModelAdmin::newAtomicCell( const CellPosition& cellPos, bool inertial, const string &modelName )
{
	AtomicCell *atomicCell ;

	if( inertial )
		atomicCell = new InertialDelayCell( cellPos, modelName ) ;
	else
		atomicCell = new TransportDelayCell( cellPos, modelName ) ;

	add2DB ( atomicCell );
	return *atomicCell ;
}

/*******************************************************************
* Function Name: newCoupled
* Description: make a new coupled model and store it in the bd
********************************************************************/
Coupled &ParallelModelAdmin::newCoupled( const string &modelName )
{
	Coupled *coupled = new Coupled( modelName ) ;

	add2DB( coupled );

	return *coupled;
}

/*******************************************************************
* Function Name: newCoupledCell
********************************************************************/
CoupledCell &ParallelModelAdmin::newCoupledCell( const string &modelName )
{
	CoupledCell *coupled = new CoupledCell( modelName ) ;

	add2DB( coupled );

	return *coupled ;
}

/*******************************************************************
* Function Name: newFlatCoupledCell
********************************************************************/
FlatCoupledCell &ParallelModelAdmin::newFlatCoupledCell( const string &modelName )
{
	FlatCoupledCell *coupled; // = new FlatCoupledCell( modelName ) ;

	add2DB( coupled );
	return *coupled ;
}

/*******************************************************************
* Function Name: totalObjectsCount
* Description: Returns the total number of DEVS processors (Warped objects) 
* that will be taking part of the simulation
********************************************************************/
int ParallelModelAdmin::totalObjectsCount() const {

	int totalObjects = 0;

	totalObjects = model("top").totalProcCount();
	totalObjects += model("ParallelRoot").totalProcCount();

	return totalObjects;
}

/*******************************************************************
* Function Name: totalObjectsCount
* Description: Returns the total number of DEVS processors (Warped objects) 
* that will be taking part of the simulation
********************************************************************/
int ParallelModelAdmin::localObjectsCount() const {

	int localObjects = 0;

	localObjects = model("top").localProcCount();
	localObjects += model("ParallelRoot").localProcCount();

	return localObjects;
}


/*******************************************************************
* Function Name: totalMachinesCount
* Description: Returns the total number of machines (Warped Logical Processes)
* that will be taking part of the simulation
********************************************************************/
int ParallelModelAdmin::totalMachinesCount() const {

	set<ProcId, less<ProcId> > machineSet;
	ParallelModelAdmin::ModelDB::const_iterator models;
	ModelPartition::const_iterator machines;

	for(models = this->models().begin(); 
	models!= this->models().end();
	models++) {

#ifndef USE_VECTOR
		Model *model = models->second;
#else
		Model *model = *models;
#endif

		for ( machines = model->modelPartition().begin(); 
		machines != model->modelPartition().end();
		machines++)

			machineSet.insert(machines->first);
	}

	return machineSet.size();

}
