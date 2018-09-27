/*******************************************************************
*
*  DESCRIPTION: Class MessageBag 
*
*  AUTHOR: Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 19/02/2001
*
*******************************************************************/

#ifndef __MESSAGEBAG_H
#define __MESSAGEBAG_H

#include <list>
#include <map>
#include <string>
#include "VTime.h"

/** forward declarations **/
class BasicPortMessage;

class MessageBag
{

public:
	typedef std::list<const BasicPortMessage*> MessageList;
private:

	typedef std::map< std::string, MessageList , std::less< std::string > > MessagesOnPort;

	MessagesOnPort msgs;
	int count;
	VTime msgtime;

public:	

	MessageBag();	//Default Constructor
	~MessageBag();

	MessageBag &add( const BasicPortMessage* );

	bool portHasMsgs( const std::string& portName ) const;

	const MessageList& msgsOnPort( const std::string& portName ) const;

	int size() const
	{return count;}

	MessageBag& eraseAll();

	const VTime& time() const
	{return msgtime;}

	struct iterator 
	{
		iterator();
		iterator( const MessagesOnPort::const_iterator &begin, const MessagesOnPort::const_iterator &end, const MessageList* currentList);
		iterator( const MessagesOnPort::const_iterator &begin, const MessagesOnPort::const_iterator &end );

		iterator &operator ++(int) ;

		const BasicPortMessage *operator *() const;


		bool operator ==( const iterator &it ) const;
		bool operator !=( const iterator &it ) const;
		iterator &operator=( const iterator &it );

		MessagesOnPort::const_iterator msgsxport, msgsEnd;
		const MessageList *currentList;
		MessageList::const_iterator msgs;


	};

	iterator begin() const;
	iterator end() const;


}; // MessageBag


/** Inline Functions **/
inline
	MessageBag::iterator::iterator() : currentList(NULL) 
{}


#endif //__MESSAGEBAG_H
