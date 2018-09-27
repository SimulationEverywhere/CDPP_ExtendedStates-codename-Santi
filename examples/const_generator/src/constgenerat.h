/*******************************************************************
*
*  DESCRIPTION: class ConstantGenerator
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __CONSTGENERATOR_H
#define __CONSTGENERATOR_H

/** include files **/
#include "atomic.h"     // class Atomic
#include "except.h"     // class InvalidMessageException

#define ATOMIC_MODEL_NAME "ConstGenerator"

/** declarations **/
class ConstGenerator : public Atomic
{
public:
	ConstGenerator( const std::string &name = ATOMIC_MODEL_NAME );				  // Default constructor

	virtual std::string className() const
		{return "ConstGenerator";}

protected:
	Model &initFunction() ;

	Model &externalFunction( const ExternalMessage & );

	Model &internalFunction( const InternalMessage & );

	Model &outputFunction( const CollectMessage & );

private:
	const Port &stop;
	Port &out ;
	VTime preparationTime;
			
};	// class ConstGenerator


#endif   //__CONSTGENERATOR_H 
