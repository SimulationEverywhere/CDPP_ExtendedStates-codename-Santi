#ifndef NOTIME_OBJECT_HH
#define NOTIME_OBJECT_HH

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
//
// $Id: 
//
//---------------------------------------------------------------------------

#include <new>
#include "BasicNoTime.h"
#include "DefaultVTime.h"
#include "BasicState.h"
#include "Interactive.h"

class LogicalProcess;
class BasicEvent;
class InputQueue;

class NoTimeObject : public BasicNoTime {
public:
  NoTimeObject();
  virtual ~NoTimeObject();

  struct NoTimeStateWrapper {
    BasicState* current;
  } *state;
  
  void executeSimulation();
  BasicEvent *getEvent();       // gets an event from the input queue
  void sendEvent(BasicEvent *); // put an event on the output queue
  void recvEvent(BasicEvent *); // how to handle incoming messages
  
  void setInputQueue(InputQueue *);
  
  void executeProcess() = 0;
  virtual void initialize() {};
  virtual void finalize() {};
  virtual void terminateSimulation(char *) {};

  virtual void deAllocateState(BasicState*);
  virtual BasicState* allocateState() = 0;
  virtual void causalityViolationHandler(BasicEvent *) {};
  
  inline VTime getLVT() const { return state->current->lVT; }

protected:
  void sendEventUnconditionally(BasicEvent*);
  virtual void fillEventInfo(BasicEvent*);

private:
  SequenceCounter eventCounter;

  void timeWarpInit();
  InputQueue* inputQueue;
};

#endif
