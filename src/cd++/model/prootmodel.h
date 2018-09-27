/*******************************************************************
*
*  DESCRIPTION: class RootModel
*
*  AUTHOR:    Alejandro Troccli
*
*  EMAIL: 	mailto://atroccol@dc.uba.ar
*
*  DATE: 	29/01/2001
*
*******************************************************************/

#ifndef __PROOTMODEL_H
#define __PROOTMODEL_H

/** include files **/
#include "model.h"   //class Model

#define	ROOT_NAME		"Root"

/** definitions **/
class RootModel : public Model
{
public:
	virtual ~RootModel()	// Destructor
	{}
	
	unsigned long totalProcCount() const
	{return 1;}
	
	unsigned long localProcCount() const;
	
protected:
	
	friend class ParallelModelAdmin;
	
	RootModel( const std::string &name = "ParallelRoot" )	// Constructor
	: Model( name )
	{}
	
	virtual std::string className() const
	{return ROOT_NAME;}
	
	virtual ParallelProcessor &createParallelProcessor()
	{ return SingleParallelProcessorAdmin::Instance().generateRoot( this, this->localProc());}
	
	
	
};	// class RootModel

inline
unsigned long RootModel::localProcCount () const {
	
	if ( localProc() != ParallelProcessor::InvalidId ) 
		return 1;
	else
		return 0;
}

#endif   //__PROOTMODEL_H
