/*******************************************************************
*
*  DESCRIPTION: class Port
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*  Version 3: Alejandro Troccoli

*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 28/2/1999 (v2)
*  DATE: 25/1/2001 (v3)
*******************************************************************/

#ifndef __PORT_H
#define __PORT_H

/** include files **/
#include <list>       // Template std::list
#include <string>    // class std::string 
#include "modelid.h"   // class ModelId
#include "pmodeladm.h"	//class SingleParallelModelAdmin

/** foward declarations **/
class Port;
class Coupled ;

/** definitions **/

typedef Port Influence ;

typedef const Influence* CPInf ;

typedef std::list<CPInf> InfluenceList;
typedef int PortId ;

class Port
{
public:
	Port( const std::string & , const ModelId & ) ;  // Default constructor

	const InfluenceList &influences() const
	{ return influenceList; }

	const std::string &name() const
	{ return pname; }

	Port &name( const std::string & ) ;

	const PortId &id() const
	{ return pid; }

	const ModelId &modelId() const
	{ return mid; }

	const Model &model() const
	{ return SingleParallelModelAdm::Instance().model(mid); }

	bool operator ==( const Port &port ) const	 // Equality operator
	{ return pid == port.id(); }

	const std::string asString() const
	{ return pname; }

	static int newPortId();

	Port &addInfluence( const Influence & );

protected:
	Port(const Port&);			// Copy constructor
	Port& operator=( const Port& );	// Assignment operator

private:

	InfluenceList influenceList;

	PortId pid ;

	std::string pname ;

	const ModelId& mid ;

	static int idCount ;

};	// class Port

#endif   //__PORT_H 
