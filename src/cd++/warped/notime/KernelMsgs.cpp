#ifndef KERNEL_MSGS_CC
#define KERNEL_MSGS_CC

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

#include "KernelMsgs.h"

using namespace std;

ostream&
operator<<(ostream& os, const KernelMsgType type) {
  switch(type) {
  case INITMSG:
    os << "INITMSG";
    break;
  case STARTMSG:
    os << "STARTMSG";
    break;
  case TERMINATEMSG:
    os << "TERMINATEMSG";
    break;
  case EVENTMSG:
    os << "EVENTMSG";
    break;
  case DUMMYMSG:
    os << "DUMMYMSG";
    break;
  default:
    os << "UNKNOWN";
  }
  
  return os;
}

ostream&
operator<<(ostream& os, const BasicMsg &e) {
  os << "sequence: "  << e.sequence
     << " senderLP: " << e.senderLP
     << " destLP: "   << e.destLP
     << " type: "     << e.type << endl;
  
  return os;
}

ostream&
operator<<(ostream& os, const InitMsg &e) {
  os << (BasicMsg) e;
  os << " objId : " << e.objId << " lpId " << e.lpId;
  
  return os;
}


ostream&
operator<<(ostream& os, const TerminateMsg &e) {
  os << (BasicMsg) e;
  os << " error : " << e.error;

  return os;
}

ostream&
operator<<(ostream& os, const CheckIdleMsg &e) {
  os << (BasicMsg) e;
  os << " tokenNum : "    << e.tokenNum << endl;
  
  return os;
}

ostream&
operator<<(ostream& os, const EventMsg &e) {
  os << (BasicMsg) e << " " << (BasicEvent) e;
  
  return os;
}

void printCasted(ostream &os, const BasicMsg *msg)  {
  switch(msg->type) {
  case INITMSG:
    os << (*(InitMsg *) msg) << endl;
    break;
    
  case STARTMSG:
    os << (*(StartMsg *) msg) << endl;
    break;
    
  case TERMINATEMSG:
    os << (*(TerminateMsg *) msg)  << endl;
    break;
    
  case EVENTMSG:
    os << (*(EventMsg *) msg) << endl;
    break;
	
  case DUMMYMSG:
    os << *(DummyMsg *) msg  << endl;
    break;
    
  default:
    os << "Error:!! printCases(ostream &, const BasicMsg *) called"
       << " with unkown event type." << endl;
    break;
  }
}

#endif
