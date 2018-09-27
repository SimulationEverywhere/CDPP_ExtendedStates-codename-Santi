#ifndef NOTIME_OBJECT_CC
#define NOTIME_OBJECT_CC

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
// $Id:
//
//---------------------------------------------------------------------------

#include "LogicalProcess.h"
#include "InputQueue.h"
#include "NoTimeObject.h"

using namespace std;

NoTimeObject::NoTimeObject() {
  inputQueue     = NULL;
  eventCounter   = 0;
  state          = new NoTimeStateWrapper;
  state->current = NULL;
}

NoTimeObject::~NoTimeObject() {
  delete state->current;
  delete state;
}

BasicEvent*
NoTimeObject::getEvent()  {
  BasicEvent* event;

  if ((event = inputQueue->getCurrentAndGotoNext()) != NULL) {
    event->alreadyProcessed = true;
    if (state->current->lVT <= event->recvTime) {
      state->current->lVT = event->recvTime;
    }
    else {
      causalityViolationHandler(event);
      //state->current->lVT++;
    }
  }
  
  return event;
}

void
NoTimeObject::sendEvent(BasicEvent* toSend)  {
  fillEventInfo(toSend);
  sendEventUnconditionally(toSend);
}

void
NoTimeObject::fillEventInfo(BasicEvent* eventToSend) {
  eventToSend->sendTime         = state->current->lVT;
  eventToSend->sender           = id;
  eventToSend->alreadyProcessed = false;
  eventToSend->eventId          = eventCounter++;
}

void
NoTimeObject::sendEventUnconditionally(BasicEvent* toSend) {
  //int receiver = toSend->dest;
  communicationHandle[toSend->dest].ptr->recvEvent(toSend);
}

void 
NoTimeObject::executeSimulation() {
  executeProcess();
}

void
NoTimeObject::recvEvent(BasicEvent *newEvent) {
  inputQueue->enQueue(newEvent);
}

void 
NoTimeObject::timeWarpInit() {
  state->current      = allocateState();
  state->current->lVT = ZERO;
  initialize();
}

void
NoTimeObject::deAllocateState(BasicState* oldState) {
  delete oldState;
}

void
NoTimeObject::setInputQueue(InputQueue* iq) {
  inputQueue = iq;
}

#endif
