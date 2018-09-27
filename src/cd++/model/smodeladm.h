/*******************************************************************
*
*  DESCRIPTION: classModelAdmin and SingleModelAdmin
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __SMODEL_ADMIN_H
#define __SMODEL_ADMIN_H

/** include files **/
#include <map>            // Template std::map
#include <string>           // Template std::string
#include "pmodeladm.h"

class StandAloneModelAdmin : public ModelAdmin
{

protected:
	friend class ParallelMainSimulator ;
	friend class ParallelMainSimulator ;
	friend class StandAloneMainSimulator ;
	friend class Coordinator ;
	friend class ProcessorAdmin ;
	friend class Root ;
	friend class CoupledCell ;	// para crear los atomicCell


	StandAloneModelAdmin( const StandAloneModelAdmin & );	// Copy constructor
//	StandAloneModelAdmin& operator =( const StandAloneModelAdmin & ) ;// Assignment operator
//	int operator ==( const StandAloneModelAdmin & ) const;	 //Equality operator

	virtual Atomic &newAtomic( const AtomicType &, const std::string &modelName );
	virtual Atomic &newAtomic( const std::string &typeNmae, const std::string &modelName );

	virtual AtomicCell &newAtomicCell( bool inertial = false, const std::string &modelName = "AtomicCell" ) ;

	virtual Coupled &newCoupled( const std::string &modelName ) ;
	virtual CoupledCell &newCoupledCell( const std::string &modelName ) ;
	virtual FlatCoupledCell &newFlatCoupledCell( const std::string &modelName ) ;


public:
	const AtomicTypes &newTypes() const ;
	StandAloneModelAdmin() ;	 // Default constructor
	virtual ~StandAloneModelAdmin(){};

};	// class StandAloneModelAdmin


#endif   //__SMODEL_ADMIN_H
