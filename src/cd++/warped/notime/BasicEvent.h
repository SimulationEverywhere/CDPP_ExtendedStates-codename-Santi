#ifndef BASICEVENT_HH
#define BASICEVENT_HH
// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.

// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 

// IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING 
// WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR 
// REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR 
// DAMAGES, INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL 
// DAMAGES ARISING OUT OF THE USE OR INABILITY TO USE THE PROGRAM 
// (INCLUDING BUT NOT LIMITED TO LOSS OF DATA OR DATA BEING RENDERED 
// INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD PARTIES OR A FAILURE OF 
// THE PROGRAM TO OPERATE WITH ANY OTHER PROGRAMS), EVEN IF SUCH HOLDER 
// OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES. 

//
// $Id:
//
//---------------------------------------------------------------------------

#include "NoTime.h"
#include "SimulationTime.h"
#include <iostream>

class BasicEvent {
	friend std::ostream& operator<<(std::ostream&, const BasicEvent &);
public:
	BasicEvent() {}

	BasicEvent(const BasicEvent*);
	BasicEvent(VTime, VTime, unsigned, unsigned, SequenceCounter, unsigned);

	virtual ~BasicEvent() {};
	virtual void print(){};

	SequenceCounter eventId;         // tuple of this and sender marks
                                   // a message as unique
	VTime sendTime;                  // send time of the event
	VTime recvTime;                  // receive time of the event
	int sender;                      // id of sending object
	int dest;                        // id of receiving object
	bool alreadyProcessed;           // Need for cleaning up stuff
	unsigned size;                   // size of this message

	BasicEvent *next;
	BasicEvent *prev;
};

#endif
