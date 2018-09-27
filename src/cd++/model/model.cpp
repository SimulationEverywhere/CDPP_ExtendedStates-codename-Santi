/*******************************************************************
*
*  DESCRIPTION: class Model
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 25/4/1999 (v2)
*  DATE: 26/9/2000 
*
*******************************************************************/

/** include files **/
#include <cctype>
#include "parsimu.h"   //Main Simulator
#include "parsimu.h"
#include "model.h"      // class Model
#include "strutil.h"    // lowerCase
#include "except.h"

using namespace std;

/** public functions **/
/*******************************************************************
* Function Name: Model
* Description: constructor default
********************************************************************/
Model::Model( const string &name )
: ident( ParallelProcessor::InvalidId )
	,local_proc( ParallelProcessor::InvalidId)
	, parent_id( ParallelProcessor::InvalidId )
	, parentModel( NULL )
	, proc( NULL )
	, descript( name )
{
}

/*******************************************************************
* Function Name: ~Model
* Description: destructor
********************************************************************/
Model::~Model()
{
	PortList::iterator cursor = inputList.begin();
	for ( ; cursor != inputList.end() ; cursor++ )
		delete cursor->second;

	for ( cursor = outputList.begin(); cursor != outputList.end(); cursor++ )
		delete cursor->second;

}

/*******************************************************************
* Function Name: addInputPort
* Post: adds the port to the input list
********************************************************************/
Port &Model::addInputPort( const string &portName )
{
	Port *pPort = new Port( lowerCase( portName ), id() );
	inputPorts()[ pPort->id() ] = pPort;
	return *pPort;
}

/*******************************************************************
* Function Name: addOuputPort
* Description: adds the port to the output list 
********************************************************************/
Port &Model::addOutputPort( const string &portName )
{
	Port *pPort = new Port( lowerCase( portName ), id() );
	outputList[ pPort->id() ] = pPort;
	return *pPort;
}

/*******************************************************************
* Function Name: port
* Description: Retorna el port solicitado
********************************************************************/
Port &Model::port( const string &n )
{
	string name( lowerCase( n ) ) ;

	PortList::iterator cursor = inputPorts().begin();

	for( ; cursor != inputPorts().end() && cursor->second->name() != name; cursor++ )
	;

	if (cursor == inputPorts().end())
	{
		for (cursor = outputPorts().begin(); cursor != outputPorts().end() && cursor->second->name() != name ; cursor++ )
		;

		if ( cursor == outputPorts().end() )
		{
			InvalidPortRequest e ;
			e.addText( name + " port not found!" ) ;
			MTHROW( e ) ;
		}
	}
	MASSERT( cursor->second );
	return *( cursor->second );
}

/*******************************************************************
* Function Name: port
* Description: Retorna el port solicitado
********************************************************************/
Port &Model::port( const PortId &pid )
{

	PortList::iterator cursor;

	cursor = inputPorts().find( pid );

	if ( cursor == inputPorts().end() ){

		cursor = outputPorts().find( pid );

		if ( cursor == outputPorts().end() )
		{
			InvalidPortRequest e ;
			e.addText( "Port with id not found!" ) ;
			MTHROW( e ) ;
		}

	}

	return *( cursor->second );
}

/*******************************************************************
* Function Name: nextChange
********************************************************************/
Model &Model::nextChange( const VTime &t )
{
	processor().nextChange(t);
	return *this;
}

/*******************************************************************
* Function Name: lastChange
********************************************************************/
Model &Model::lastChange( const VTime &t )
{
	processor().lastChange(t);
	return *this;
}
/*******************************************************************
* Functions that will be used by parallel DEVS Processors
********************************************************************/

/*******************************************************************
* Function Name: addMachine
* Description: indicates a new machine where the model will be executing
********************************************************************/

Model &Model::addMachine( const MachineId &mid )
{

	//Add new machine only if it did not previously exist.

	ModelPartition::const_iterator cursor;

	for(cursor = model_partition.begin(); cursor != model_partition.end(); cursor++) {

		if ( cursor->first == mid )
			break;

	}

	if ( cursor == model_partition.end() ) 
		model_partition[ mid ] = ParallelProcessor::InvalidId;	

	return *this;
}



/*******************************************************************
* Function Name: machineForProcId
* Returns the machine where the proc processor is running
********************************************************************/	

const MachineId &Model::machineForProcId( const ProcId &proc ) const {

	ModelPartition::const_iterator cursor;

	for( cursor = model_partition.begin(); cursor != model_partition.end(); cursor++ )
	{
		if ( cursor->second == proc)
			return cursor->first;
	}	

	MException e;
	e.addText( string(" There is no processor with id: ") + proc + string(" in model ") + description() );
	MTHROW ( e );

}

/*******************************************************************
* Function Name: isLocalMaster
* Returns true if the local processor for the model is a Master processor.
********************************************************************/	
bool Model::isLocalMaster() const
{

	ModelPartition::const_iterator cursor;

	cursor = model_partition.begin();

	if (cursor != model_partition.end()) {
		int mid = ParallelMainSimulator::Instance().getMachineID();
		if (mid == cursor->first) 
			return true;
	}

	return false;

}

/*******************************************************************
* Function Name: masterId
* Returns the Id of the master processor
********************************************************************/	
const ProcId &Model::masterId() const 
{
	ModelPartition::const_iterator cursor;

	cursor = model_partition.begin();

	if (cursor != model_partition.end()) {
		return cursor->second;
	} 

	return ParallelProcessor::InvalidId;

}

/*******************************************************************
* Function Name: procInMachine
* Returns the procId of the DEVS processor that is running on machine
* with id mid. If no processor is running on that machine, returns InvalidId
********************************************************************/	
const ProcId &Model::procInMachine( const MachineId &mid) const
{

	ModelPartition::const_iterator cursor = model_partition.find(mid);

	if (cursor != model_partition.end())
		return cursor->second;

	return ParallelProcessor::InvalidId;

}

/*******************************************************************
* Function Name: processor
* Returns the ParallelProcessor that is associated with the model
* If no processor should run on this machine, an exception is thrown.
* If the processor has not yet been created, an exception is thrown.
********************************************************************/	
ParallelProcessor &Model::processor() const
{

	if (localProc() == ParallelProcessor::InvalidId) {
		InvalidProcessorIdException e;
		e.addText( string( "The model " + description() +  " does not have a processor on this machine")) ;
		MTHROW( e ) ;	
	}


	return *proc;

}

/*******************************************************************
* Function Name: processor
* Sets the ParallelProcessor that is associated with the model
* If no processor should run on this machine, an exception is thrown.
* If the processor has not yet been created, an exception is thrown.
********************************************************************/	
Model &Model::processor( ParallelProcessor *p ) 
{

	if (localProc() == ParallelProcessor::InvalidId) {
		InvalidProcessorIdException e;
		e.addText( string( "The model " + description() +  " does not have a processor on this machine")) ;
		MTHROW( e ) ;	
	}

	proc = p;
	return *this;

}

/*******************************************************************
* Function Name: setMachineProcId
* Sets the Warped Id for a specific machine
********************************************************************/	
Model &Model::setMachineProcId( const MachineId &mid, const ProcId &pid) {

	if (mid == ParallelMainSimulator::Instance().getMachineID())
		local_proc = pid;

	model_partition[ mid ] = pid;	

	//Register the model and ProcId in the ProcId->model mapping
	SingleParallelProcessorAdmin::Instance().model( pid, this);

	return *this;
}
