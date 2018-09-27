#ifndef KERNEL_MSGS_HH
#define KERNEL_MSGS_HH

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

#include "BasicEvent.h"

enum KernelMsgType { INITMSG, STARTMSG, TERMINATEMSG,
		     CHECKIDLEMSG, EVENTMSG, DUMMYMSG, DEBUGMSG };

extern std::ostream& operator<<(std::ostream&, const KernelMsgType);

class BasicMsg {
  friend std::ostream& operator<<(std::ostream&, const BasicMsg &);
public:
  BasicMsg(){};
  BasicMsg(KernelMsgType kMT) : type(kMT) {};
  SequenceCounter sequence;  // needed to avoid message acknowledgment in the
                             // general case
  int senderLP;
  int destLP;
  KernelMsgType type;
};

class InitMsg : public BasicMsg {
  friend std::ostream& operator<<(std::ostream&, const InitMsg &);
public:
  InitMsg() : BasicMsg(INITMSG) {}
  ~InitMsg() {}
  
  int objId; // this is id of the object I'm telling you about
  int lpId;  // the LP that has it
};

class StartMsg : public BasicMsg {
public:
  StartMsg() : BasicMsg(STARTMSG){}
  ~StartMsg() {}
};

class CheckIdleMsg : public BasicMsg {
  friend std::ostream & operator<<(std::ostream&, const CheckIdleMsg & );
public:
  CheckIdleMsg() : BasicMsg(CHECKIDLEMSG) {}
  int tokenNum;
};

class TerminateMsg : public BasicMsg {
  friend std::ostream& operator<<(std::ostream&, const TerminateMsg &);
public:
  TerminateMsg() : BasicMsg(TERMINATEMSG) {}
  char error[255];
};

class EventMsg : public BasicMsg, public BasicEvent {
  friend std::ostream& operator<<(std::ostream&, const EventMsg&);
public:
  EventMsg() : BasicMsg(EVENTMSG) {}
  ~EventMsg() {}
};

struct DummyMsg : public BasicMsg {
public:
  DummyMsg() : BasicMsg(DUMMYMSG){}
  ~DummyMsg() {}
};

class BasicDebugMsg : public BasicMsg {
public:
  BasicDebugMsg() : BasicMsg(DEBUGMSG) {}
  ~BasicDebugMsg() {}

  int size;
};

#endif
