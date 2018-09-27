/*******************************************************************
*
*  DESCRIPTION: classModelAdmin and SingleModelAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian
*	   Alejandro Troccoli (v3)
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  	 27/9/2000 (v3)
*
*******************************************************************/

#ifndef __MODEL_ADMIN_H
#define __MODEL_ADMIN_H

/** include files **/
#include <map>            // Template std::map
#include <string>           // Template std::string
#include "except.h"

/** foward declarations **/
class ModelAdmin;
class SingleModelAdm ;
class Atomic ;
class AtomicCell ;
class Coupled ;
class CoupledCell ;
class Model ;
class ProcessorAdmin ;
class Root ;
class ParallelMainSimulator ;
class FlatCoupledCell ;

/** definitions **/

struct NewFunction
{
	virtual Atomic *operator()( const std::string &  ) = 0 ;   
	virtual NewFunction *duplicate() const = 0 ;
};

template <class T> 
struct NewAtomicFunction : NewFunction
{
	Atomic *operator()( const std::string &name )
	{return new T( name ) ;}
	NewFunction *duplicate() const
	{return new NewAtomicFunction<T>() ;}
} ;


class ModelAdmin
{
public:   
	typedef int AtomicType ;
	
	AtomicType registerAtomic( const NewFunction &f, const std::string & );
	
	//Functions for accesing the ModelDB.
	typedef std::map< std::string, Model *, less < std::string > > ModelDB;
	
	const ModelDB &models() const;
	virtual Model& model( const std::string modelName );	
	
	ModelAdmin();
	virtual ~ModelAdmin(){};
	
protected:
	friend class ParallelMainSimulator ;
	friend class StandAloneMainSimulator ;
	friend class Coordinator ;
	friend class ProcessorAdmin ;
	friend class Root ;
	friend class ParallelRoot;
	friend class CoupledCell ;	// para crear los atomicCell
	friend class SingleModelAdm ;
	
	ModelAdmin( const ModelAdmin & );	// Copy constructor
//	virtual ModelAdmin& operator =( const ModelAdmin & ) = 0 ; //Assignment operator
//	virtual int operator ==( const ModelAdmin & ) const = 0; //Equality operator
	
	virtual Atomic &newAtomic( const AtomicType &, const std::string &modelName ) = 0;
	virtual Atomic &newAtomic( const std::string &typeNmae, const std::string &modelName ) = 0;
	
	virtual AtomicCell &newAtomicCell( bool inertial = false, const std::string &modelName = "AtomicCell" ) = 0;
	
	virtual Coupled &newCoupled( const std::string &modelName ) = 0;
	virtual CoupledCell &newCoupledCell( const std::string &modelName ) = 0;
	virtual FlatCoupledCell &newFlatCoupledCell( const std::string &modelName ) = 0;
	
	typedef std::map< std::string, AtomicType, std::less< std::string > > AtomicTypes ;
	typedef std::map< AtomicType, NewFunction*, std::less< AtomicType > > AtomicKinds ;
	
	AtomicKinds kinds  ;
	AtomicType  typeCount ;
	AtomicTypes types ;
	
	//Functions for accesing the Model Database
	ModelAdmin& add2DB( Model * );
	
private:
	
	ModelDB modelDb;
	
public: 
	const AtomicTypes &newTypes() const ;
};	// class ModelAdmin

class SingleModelAdm
{
public:
	static ModelAdmin &CreateParallel();
	static ModelAdmin &CreateStandAlone();
	static ModelAdmin &Instance() ;
private:
	static ModelAdmin *instance ;
	SingleModelAdm() ;
};	// SingleModelAdm

/** inline methods **/

inline
	SingleModelAdm::SingleModelAdm()
{}

inline
	ModelAdmin &SingleModelAdm::Instance()
{
	MASSERTMSG( instance, "No SingleModelAdm (Parallel or StandAlone) has been defined." );
	return *instance;
}

#endif   //__MODEL_ADMIN_H 
