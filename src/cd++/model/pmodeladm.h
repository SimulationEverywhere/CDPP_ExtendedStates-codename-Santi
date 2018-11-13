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

#ifndef __PMODEL_ADMIN_H
#define __PMODEL_ADMIN_H


/** include files **/
#include <vector>
#include <map>

// If OS is Apple based, hashmap is defined differently
#if defined __GNUC__ || defined __APPLE__
       #include <ext/hash_map>
#else
       #include <hash_map>
#endif

#include <string>           // Template std::string
#include "strutil.h"		// hash<std::string>
#include "modelid.h"
#include "cellpos.h"

/** foward declarations **/
class ParallelModelAdmin;
class SingleParallelModelAdm ;
class Model ;
class Atomic ;
class AtomicCell ;
class Coupled ;
class CoupledCell ;
class FlatCoupledCell ;
class ParallelProcessorAdmin ;
class ParallelRoot ;
class ParallelMainSimulator ;

/** definitions **/
using namespace __gnu_cxx;

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


class ParallelModelAdmin
{
public:   
	typedef int AtomicType ;

	AtomicType registerAtomic( const NewFunction &f, const std::string & );

	//Functions for accesing the ModelDB.
#ifndef USE_VECTOR
	typedef hash_map< ModelId, Model * > ModelDB;
#else
	typedef std::vector<Model*> ModelDB;
#endif
	typedef hash_map< std::string, Model *,HashString > ModelDic;

	const ModelDB &models() const;

	Model& model( const std::string& modelName ) const;	
	Model& model( const ModelId model ) const;

	ParallelModelAdmin();
	virtual ~ParallelModelAdmin();

protected:
	friend class ParallelMainSimulator ;
	friend class ParallelProcessorAdmin ;
	friend class ParallelRoot ;
	friend class Coordinator ;	
	friend class CoupledCell ;	// para crear los atomicCell

	typedef std::map< std::string, AtomicType, std::less <std::string> > AtomicTypes ;
	typedef std::map< AtomicType, NewFunction*, std::less< AtomicType > > AtomicKinds ;

	ParallelModelAdmin( const ParallelModelAdmin & );	// Copy constructor

	virtual Model &newRoot();

	virtual Atomic &newAtomic( const AtomicType &, const std::string &modelName );
	virtual Atomic &newAtomic( const std::string &typeNmae, const std::string &modelName );

	virtual AtomicCell &newAtomicCell( const CellPosition& cellPos, bool inertial = false, const std::string &modelName = "AtomicCell" ) ;

	virtual Coupled &newCoupled( const std::string &modelName ) ;
	virtual CoupledCell &newCoupledCell( const std::string &modelName ) ;
	virtual FlatCoupledCell &newFlatCoupledCell( const std::string &modelName );

	int totalObjectsCount() const;
	int localObjectsCount() const;
	int totalMachinesCount() const;

	ModelId newModelId() 
	{return modelCount++;}

	//Functions for accesing the Model Database
	ParallelModelAdmin& add2DB( Model * );

private:

	AtomicType  typeCount ;	
	ModelId modelCount;

	AtomicKinds kinds  ;
	AtomicTypes types ;

	ModelDB  modelDb;
	ModelDic modelDic;



public:
	const AtomicTypes &newTypes() const ;

};	// class ParallelModelAdmin

class SingleParallelModelAdm
{
public:
	static ParallelModelAdmin &Instance() ;

private:
	static ParallelModelAdmin *instance ;
	SingleParallelModelAdm() ;
};	// SingleModelAdm


/** inline methods **/
inline
	SingleParallelModelAdm::SingleParallelModelAdm()
{}

inline
	ParallelModelAdmin &SingleParallelModelAdm::Instance()
{
	if( !instance ) {

		instance = new ParallelModelAdmin();        	
	}
	return *instance;

}


#endif   //__PMODEL_ADMIN_H
