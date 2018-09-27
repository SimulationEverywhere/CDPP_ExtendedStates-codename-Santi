/*******************************************************************
*
*  DESCRIPTION: class Distribution (abstract)
*               class ChiDistribution
*               class PoissonDistribution
*               class NormalDistribution
*               class ConstantDistribution
*               class ExponentialDistribution
*               class InvalidDistribution (exception)
*
*  AUTHOR: Amir Barylko & Jorge Beyoglonian 
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*
*  DATE: 27/6/1998
*
*******************************************************************/

#ifndef __DISTRIBUTION_H
#define __DISTRIBUTION_H

/** include files **/
#include <string>
#include "except.h"        // class MException

/** declarations **/
class Distribution
{
public:
	static Distribution *create( const std::string &distributionName );

	virtual ~Distribution()			//Destructor
		{}
		
	virtual int varCount() const = 0;
	const std::string getVar(unsigned int) const;
	Distribution &setVar(unsigned int, float);
	virtual float get() = 0;

protected:
	virtual const std::string get(unsigned int) const = 0;
	virtual Distribution &set(unsigned int, float) = 0;
	Distribution();	 //Default constructor
};	// class Distribution

class InvalidDistribution: public MException
{} ;

/*******************************************************************
* class Name: ChiDistribution
********************************************************************/
class ChiDistribution : public Distribution
{
public:
	int varCount() const
		{return 1;}
		
	float get();

protected:
	const std::string get( unsigned int ) const
		{return "degreesfreedom";}
	        
	Distribution &set( unsigned , float value)
		{ df = value; return *this; }

private:
	float df ;	 //Degrees of Freedom
};	// class ChiDistribution


/*******************************************************************
* class Name: NormalDistribution
********************************************************************/
class NormalDistribution: public Distribution
{
public:
	int varCount() const
		{return 2;}
		
	float get();

protected:
	const std::string get( unsigned int index) const
		{return  index == 0?"mean":"deviation";}

	Distribution &set( unsigned int, float ) ;

private:
	float mean ;
	float deviation ;

};	// class NormalDistribution


/*******************************************************************
* class Name: ConstantDistribution
********************************************************************/
class ConstantDistribution: public Distribution
{
public:
	int varCount() const
		{return 1;}

	float get()
		{return value;}
		
protected:
	const std::string get( unsigned int ) const
		{return "value";}
		
	Distribution &set( unsigned int, float ) ;

private:
	float value ;
};	// class ConstantDistribution


/*******************************************************************
* class Name: PoissonDistribution
********************************************************************/
class PoissonDistribution : public Distribution
{
public:
	int varCount() const
		{return 1;}
	
	float get();

protected:
	const std::string get( unsigned int ) const
		{return "mean";}
		
	Distribution &set( unsigned int, float ) ;

private:
	float mu ;
};	// class PoissonDistribution


/*******************************************************************
* class Name: ExponentialDistribution
********************************************************************/
class ExponentialDistribution : public Distribution
{
public:
	int varCount() const
		{return 1;}
		
	float get();

protected:
	const std::string get( unsigned int ) const
		{return "mean";}
		
	Distribution &set( unsigned int, float ) ;

private:
	float av ;
};	// class ExponentialDistribution


/** inline **/
inline
Distribution &ConstantDistribution::set( unsigned int, float v )
{
	value = v ;
	return *this ;
}

inline
Distribution &PoissonDistribution::set( unsigned int, float v )
{
	mu = v ;
	return *this ;
}

inline
Distribution &ExponentialDistribution::set( unsigned int, float v )
{
	av = v ;
	return *this ;
}

#endif   //__DISTRIBUTION_H 
