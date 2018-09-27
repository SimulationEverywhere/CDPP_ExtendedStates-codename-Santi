/*******************************************************************
*
*  DESCRIPTION: class Coupled
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
*  DATE: 27/2/1999 (v2)
*  DATE: 4/10/2000 (v3)
*
*******************************************************************/

#ifndef __COUPLED_H
#define __COUPLED_H

/** include files **/
#include <list>    // Template std::list
#include "model.h"    // Base class Model

/** foward declarations **/
class SingleModelAdmin;
class ParallelMainSimulator ;

#define	COUPLED_NAME	"Coupled"

/** definitions **/
class Coupled : public Model 
{
public:
	Model &addModel( Model & );
	
	
	//ModelList as std::list<ModelId> is used by the StandAlone version of NCD++
	typedef std::list<ModelId> ModelList;
	
	const ModelList &children() const
	{return childs;}
	
	//For the parallel version, PModelList will be defined
	typedef std::list< Model *> PModelList;
	
	const PModelList &childModels() const
	{return pchilds;}
	
	enum Type
	{
		cell,
		regular
	} ;
	
	virtual Type type() const
	{return regular;}
	
	virtual std::string className() const
	{return COUPLED_NAME;}
	
	unsigned long totalProcCount() const;
	
	unsigned long localProcCount() const;
	
	
protected:
	friend class SingleParallelModelAdmin ;
	friend class ParallelModelAdmin ;
	friend class ParallelMainSimulator ;
	
	Coupled( const std::string &name = "Coupled" )	// Default constructor
	: Model( name )
	{}
	
	Coupled( const Coupled & )  ;		 // Copy constructor
	
	virtual Model &addInfluence( const std::string &sourceName, const std::string &sourcePort, const std::string &destName, const std::string &destPort) ;
	
	virtual ParallelProcessor &createParallelProcessor()
	{ return SingleParallelProcessorAdmin::Instance().generateProcessor(this, localProc());}
	
private:
	ModelList childs ;
	PModelList pchilds;
	
} ; // class Coupled


#endif   //__COUPLED_H 
