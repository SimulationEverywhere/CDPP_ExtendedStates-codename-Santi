/*******************************************************************
*
*  DESCRIPTION: class MessageAdmin, SingleMsgAdmin
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 28/2/1999 (v2)
*
*******************************************************************/

#ifndef __MESSAGE_ADMIN_H
#define __MESSAGE_ADMIN_H

/** include files **/
#include <utility>
#include <list>
#include "message.h"    // class Message 

/** foward declarations **/
class MessageAdmin ;
class SingleMsgAdm ;

/** definitions **/

class MessageAdmin 
{
public:
	virtual ~MessageAdmin();	//Destructor
	MessageAdmin &send( const Message &, const ModelId & );
	MessageAdmin &run();
	MessageAdmin &stop();

protected:
	MessageAdmin( const MessageAdmin & );	//Copy constructor
	MessageAdmin &operator =( const MessageAdmin & );	 // Assignment operator
	int operator ==( const MessageAdmin & ) const ;	  // Equality operator

private:
	friend class SingleMsgAdm ;

	bool running ;
	MessageAdmin();	// Default constructor
	typedef pair< Message*, ProcId > UnprocessedMsg ;
	typedef std::list< UnprocessedMsg > UnprocessedMsgQueue ;
	UnprocessedMsgQueue unprocessedQueue;

	bool isRunning() const
			{return running;}

};	// class MessageAdmin

class SingleMsgAdm
{
public:
	static MessageAdmin &Instance() ;

private:
	static MessageAdmin *instance ;
	// ** Constructors ** //
	SingleMsgAdm(){}
};	// SingleMsgAdm

/** inline methods **/
inline
MessageAdmin &SingleMsgAdm::Instance() 
{
	if( !instance )
		instance = new MessageAdmin() ;

	return *instance ;
}

#endif   //__MESSAGE_ADMIN_H 
