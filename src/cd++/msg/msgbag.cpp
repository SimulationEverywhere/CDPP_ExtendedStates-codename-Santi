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

#include "msgbag.h"
#include "message.h"
#include "except.h"

using namespace std;

/*******************************************************************
* Function Name: Constructor
********************************************************************/
MessageBag::MessageBag() : count(0)
	, msgtime(VTime::Inf)
{}

/*******************************************************************
* Function Name: Destructor
********************************************************************/

MessageBag::~MessageBag() 
{
	eraseAll();
}

/*******************************************************************
* Function Name: add
********************************************************************/
MessageBag &MessageBag::add( const BasicPortMessage* msg) 
{
	MASSERT( count == 0 || (count != 0 && msg->time() == msgtime ));

	if (count == 0) msgtime = msg->time();

	msgs[msg->port().name()].push_back( msg );
	count++;

	return *this;
}

/*******************************************************************
* Function Name: eraseAll
********************************************************************/
MessageBag &MessageBag::eraseAll()
{
	//Delete all the messages
	MessagesOnPort::const_iterator cursor;
	MessageList::const_iterator portmsgs;

	for( cursor = msgs.begin(); cursor != msgs.end(); cursor++)
	{
		for( portmsgs = cursor->second.begin(); portmsgs != cursor->second.end(); 
		portmsgs++)
		{
			delete (*portmsgs);
		}
	}

	msgs.erase( msgs.begin(), msgs.end());

	count = 0;
	msgtime = VTime::Inf;

	return *this;
}


/*******************************************************************
* Function Name: msgsOnPort
********************************************************************/
bool MessageBag::portHasMsgs( const string& portName) const
{
	if( msgs.find(portName) == msgs.end() )
		return false;
	else
		return true;

}

/*******************************************************************
* Function Name: msgsOnPort
********************************************************************/

const MessageBag::MessageList& MessageBag::msgsOnPort( const string& portName ) const
{
	MessagesOnPort::const_iterator cursor;

	cursor = msgs.find(portName);

	MASSERTMSG( cursor != msgs.end(), "Requested messages for a port that does not have any message!");

	return cursor->second;
}


/*******************************************************************
* Function Name: begin
********************************************************************/
MessageBag::iterator MessageBag::begin() const
{
	iterator it( msgs.begin(), msgs.end() );	
	return it;
}

/*******************************************************************
* Function Name: end
********************************************************************/
MessageBag::iterator MessageBag::end() const
{
	iterator it( msgs.end(), msgs.end() );	
	return it;
}


/*******************************************************************
* Function Name: iterator::iterator
********************************************************************/
MessageBag::iterator::iterator( const MessageBag::MessagesOnPort::const_iterator &begin, const MessageBag::MessagesOnPort::const_iterator &end, const MessageBag::MessageList* currList)
: msgsxport(begin)
	, msgsEnd(end)
	,currentList( currList )
{};

/*******************************************************************
* Function Name: iterator::iterator
********************************************************************/
MessageBag::iterator::iterator( const MessageBag::MessagesOnPort::const_iterator &begin, const MessageBag::MessagesOnPort::const_iterator &end)
: msgsxport(begin)
	, msgsEnd(end)
{


	if ( msgsxport != msgsEnd )
	{
		currentList = &(msgsxport->second);
		msgs = currentList->begin();
	}

	else
		currentList = NULL;

};


/*******************************************************************
* Function Name: iterator::operator++
********************************************************************/
MessageBag::iterator &MessageBag::iterator::operator ++(int) 
{
	msgs++;

	if ( msgs == currentList->end()) {

		msgsxport++;


		if ( msgsxport == msgsEnd )
		{
			currentList = NULL;
		} 
		else
		{
			currentList = &(msgsxport->second);
			msgs = currentList->begin();
		}


	}	

	return *this;
}

/*******************************************************************
* Function Name: iterator::operator++
********************************************************************/
const BasicPortMessage *MessageBag::iterator::operator *() const
{
	return (*msgs);	
}

/*******************************************************************
* Function Name: iterator::operator==
********************************************************************/
bool MessageBag::iterator::operator ==( const iterator &it ) const
{
	if ( currentList == NULL && it.currentList == NULL)
		return true;
	else if (currentList == NULL || it.currentList == NULL )
		return false;
	else
	{
		//If they point to the same object, they are equal.
		return msgs == it.msgs;
	}
}

bool MessageBag::iterator::operator !=( const iterator &it ) const
{
	return !(*this == it);
}

/*******************************************************************
* Function Name: iterator::operator==
********************************************************************/
MessageBag::iterator &MessageBag::iterator::operator=( const iterator &it )
{
	msgsxport = it.msgsxport;
	msgsEnd = it.msgsEnd;
	currentList = it.currentList;
	msgs = it.msgs;

	return *this;
}

