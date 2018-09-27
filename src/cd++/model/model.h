/*******************************************************************
*
*  DESCRIPTION: class Model(base class for Amtomic and Coupled)
*
*  AUTHOR:    Amir Barylko & Jorge Boyoglonian 
*  Version 2: Daniel Rodriguez
*  Version 3: Alejandro Troccoli
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	   mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 27/2/1999 (v2)
*
*******************************************************************/

#ifndef __MODEL_H
#define __MODEL_H

/** include files **/
#include <string>    // Template std::string
#include <map>
#include "machineid.h" //type MachineId
#include "port.h"     // class Port 
#include "VTime.h"     // class VTime
#include "portlist.h"
#include "pprocadm.h"
#include "modelstate.h"	//class ModelState
#include "value.h"

/** foward declarations **/

class ModelAdmin ;
class MessageAdmin ;
class Coupled ;
class Root ; 
class Simulator ;
class ParallelSimulator;
class Processor ;
class ParallelProcessor;
class Coordinator ;
class SendPortNode ;

/** type definitions **/

typedef std::map<MachineId, ProcId, std::less <MachineId> > ModelPartition;

/** definitions **/

#define	MODEL_NAME	"Model"


class Model
{
public:
	virtual ~Model();	// Destructor

	virtual std::string className() const = 0;

	virtual Port &addInputPort( const std::string & );
	virtual Port &addOutputPort( const std::string & );

	virtual const PortList &inputPorts() const
	{return inputList;}

	virtual const PortList &outputPorts() const
	{return outputList;}

	virtual PortList &inputPorts()
	{return inputList;}

	virtual PortList &outputPorts()
	{return outputList;}

	Port & port( const std::string & );

	Port & port( const PortId & );

	const VTime &nextChange() const
	{return processor().nextChange();}

	const VTime &lastChange() const
	{return processor().lastChange();}

	const ModelId &id() const
	{return ident;}

	const std::string &description() const
	{return descript;}

	const std::string asString() const
	{return description() + "(" + id() + ")";}

	const Model *parent() const
	{return parentModel;}

	Model &parent( Model *p )
	{parentModel = p; return *this;}

	bool operator <( const Model &model ) const
	{return this->id() < model.id();}


	// Model Partition Management Methods

	Model &addMachine( const MachineId &mid );

	Model &setMachineProcId( const MachineId &mid, const ProcId &pid);

	const ProcId &procInMachine( const MachineId &mid) const;

	const MachineId& machineForProcId( const ProcId &proc ) const;

	const ProcId &localProc() const
	{ return local_proc;}

	const ProcId &masterId() const;

	bool isLocalMaster() const;

	const ModelPartition &modelPartition() const 
	{ return model_partition; }	

	//Id of the parent processor
	const ProcId &parentId() const
	{return parent_id;}

	Model &parentId( const ProcId &mid )
	{parent_id = mid; return *this;}

	virtual unsigned long totalProcCount() const = 0;

	virtual unsigned long localProcCount() const = 0;

protected:

	friend class Processor ;
	friend class ParallelProcessorAdmin;
	friend class ParallelProcessor;
	friend class Simulator ;
	friend class ParallelMCellCoordinator;
	friend class ParallelSCellCoordinator;
	friend class ParallelSimulator;
	friend class SendPortNode ;
	friend class ParallelMainSimulator;
	friend class ParallelRoot;
	friend class ParallelModelAdmin;

	Model( const std::string &name = "Model" ) ; // Default constructor

	Model &nextChange( const VTime & );
	Model &lastChange( const VTime & );

	Model &id( const ModelId &mid )
	{ident = mid; return *this;}

	Model &sendOutput( const VTime &time, const Port &port, const double &value )
	{processor().sendOutput( time, port, Real(value) ); return *this;}

	Model &sendOutput( const VTime &time, const Port &port, const AbstractValue &value )
	{processor().sendOutput( time, port, value ); return *this;}

	Model &sendOutput( const VTime &time, const Port & port , BasicMsgValue *value)
	{processor().sendOutput( time, port, value ); return *this;}

	ParallelProcessor &processor() const;

	Model &processor( ParallelProcessor * );

	virtual ModelState* allocateState()
	{return new ModelState;}


	virtual ParallelProcessor& createParallelProcessor() = 0;

	//This function will be called for each model after the processor
	//has been initialized. This means each model with an associated
	//has it's state available. It should be used, for instance, to
	//load the initial values for cells, or the external events for
	//the root.
	virtual void afterProcessorInitialize(){};

private:
	friend class ModelAdmin ;
	friend class MessageAdmin ;
	friend class Coupled ;
	friend class Root ;

	ModelId ident ;
	ProcId local_proc;			//For efficient retrieval
	ProcId parent_id ;

	Model *parentModel;
	ParallelProcessor *proc;

	std::string descript ;

	ModelPartition model_partition;

	PortList inputList ;
	PortList outputList ;

};	// class Model


#endif   //__MODEL_H 
