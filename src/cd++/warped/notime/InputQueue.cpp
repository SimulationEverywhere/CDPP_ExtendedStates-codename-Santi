#ifndef INPUT_QUEUE_CC
#define INPUT_QUEUE_CC

// Copyright (c) 1994-1996 Ohio Board of Regents and the University of
// Cincinnati.  All Rights Reserved.
//
// BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY 
// FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT 
// PARTIES PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, 
// EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
// PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE 
// PROGRAM IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU ASSUME 
// THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION. 
//
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
//
// $Id:
//
//---------------------------------------------------------------------------

#include "InputQueue.h"
#include "BasicEvent.h"

using namespace std;

InputQueue::InputQueue() {
  head = tail = currentPos = NULL;
}

InputQueue::~InputQueue() {
  removeAllElements();
}

void
InputQueue::enQueue(BasicEvent* newEvent) {
  if (head == NULL) {
    head = tail = currentPos = newEvent;
    head->prev  = head->next = NULL;
  }
  else {
    tail->next     = newEvent;
    newEvent->prev = tail;
    newEvent->next = NULL;
    tail           = newEvent;
    
    if (currentPos == NULL) {
      currentPos = newEvent;
    }
  }
}

BasicEvent*
InputQueue::deQueue() {
  BasicEvent* returnEvent = head;
  
  if (head != NULL) {
    if ((head = head->next) != NULL) {
      head->prev = NULL;
    }
    else {
      currentPos = tail = NULL;
    }
  }

  return returnEvent;
}

BasicEvent*
InputQueue::getHead() const {
  return head;
}

BasicEvent*
InputQueue::getTail() const {
  return tail;
}

BasicEvent*
InputQueue::getCurrent() const {
  return currentPos;
}

BasicEvent*
InputQueue::getCurrentAndGotoNext() {
  BasicEvent* retVal;
  
  return ((currentPos != NULL) ? ((retVal = currentPos),
				  (currentPos = currentPos->next), retVal) :
	  currentPos);
}

BasicEvent*
InputQueue::removeAlreadyProcessedEvents() {
  for(register BasicEvent* deleteEvent = head; ((deleteEvent != NULL) && (deleteEvent->alreadyProcessed == true)); ((head = head->next), (delete [] (char *) deleteEvent), (deleteEvent = head)));

  return ((head != NULL) ? (head->prev = NULL, head) :
	  (tail = currentPos = head));
}

void
InputQueue::removeAllElements() {
  for(register BasicEvent* deleteEvent = head; (head != NULL); ((head = head->next), (delete [] (char *) deleteEvent), (deleteEvent = head)));
}

void
InputQueue::printList(ostream& os) {
  for(register BasicEvent* iterator = head; (iterator != NULL); iterator = iterator->next) {
    cout << *iterator << endl;
  }
}

#endif


