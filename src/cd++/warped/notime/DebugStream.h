#ifndef DEBUGSTREAM_HH
#define DEBUGSTREAM_HH

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

#include "NoTimeObject.h"
#include "KernelMsgs.h"
#include <fstream>
#include <sstream>

class CommManager;
class LogicalProcess;

#define PROMPT "NoTime> "

enum DebugMessageKind { CONSOLE, ERROR, DEBUG, DONE_MESSAGE, PAUSE_MESSAGE,
			RESUME_MESSAGE, DEBUG_MESSAGE, BREAK_POINT_MESSAGE,
			INITIALIZE_MESSAGE, PARTITION_MESSAGE, TIME_MESSAGE, 
			NO_MESSAGE};

class DebugMessage : public BasicDebugMsg {
public:
  DebugMessage() {};
  ~DebugMessage() {};
  
  DebugMessageKind messageKind;
};

class DebugStream : public NoTimeObject {
  friend DebugStream& operator << (DebugStream&, const char *);
  friend DebugStream& operator << (DebugStream&, const int);
  friend DebugStream& operator << (DebugStream&, const long long);
  friend DebugStream& operator << (DebugStream&, std::ostream& (*)(std::ostream&));
  friend DebugStream& operator << (DebugStream&, const float);
public:
  DebugStream(const DebugMessageKind);
  virtual ~DebugStream();
  virtual void initialize();
  virtual void finalize();
  virtual void executeProcess();
  virtual void receiveMessage(BasicDebugMsg*);
  virtual void sendDoneMessage();

  void setLPid(const int, const int, LogicalProcess*);
  void setid(const int);
  void setCommManager(CommManager*);
  
  BasicState* allocateState();

protected:
  virtual void sendDebugMessage(const char*);
  virtual void writeMessageToFile(const char *);
  virtual void checkAndSend();
  virtual void displayMessage(const char *);
  virtual void waitForDoneMsgs();
  virtual void remoteSend(DebugMessage *);
  virtual void interact();
  virtual void setBreakPoint();
  virtual void pause();
  virtual void sendMessage(int, const DebugMessageKind);
  virtual void sendPartitionInformation();
  virtual void findTime();
  virtual void sendTimeInfo(char *);
  virtual void findTimeForObject(int);

  BasicMsg* auxRecvMPIMsg();

  std::ostringstream  tempDebugStream;  
  std::fstream    debugFile;
  CommManager *communicationSystem;
  
  int        lpId;
  int        totalNoOfLps;
  int        doneReceivedSoFar;
  bool       debugFlag;
  bool       displayMessageFlag;
  bool       writeMessageToFileFlag;
  int        initializeFlag;
  bool       pauseFlag;
  int        partitionInfoCounter;
private:
  DebugMessageKind myKind;
  LogicalProcess*  lpHandle;
  DebugMessageKind lastMessage;
};

#endif
