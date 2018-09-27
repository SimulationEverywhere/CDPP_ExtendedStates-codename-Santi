#ifndef DEBUGSTREAM_CC
#define DEBUGSTREAM_CC

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

#include "LogicalProcess.h"
#include "DebugStream.h"
#include "BasicState.h"
#include "ObjectRecord.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

static char interactiveConsoleStr[] = "InteractiveConsole";
DebugStream::DebugStream(const DebugMessageKind kind) {
/*  ostringstream debugFileStream;*/
/*  char*      debugFileName;*/

  debugFlag = true;
  lpId      = -1;
  
  displayMessageFlag     = true;
  writeMessageToFileFlag = true;
  myKind                 = kind;
  doneReceivedSoFar      = 0;
  initializeFlag         = 0;
  name                   = interactiveConsoleStr;
}

void
DebugStream::remoteSend(DebugMessage* msg) {
#ifdef MPI  
  msg->senderLP = lpId;
  msg->sequence = 0;

  MPI_Bsend(msg, msg->size, MPI_BYTE, msg->destLP,
	    msg->type, MPI_COMM_WORLD);
#endif
}

BasicMsg*
DebugStream::auxRecvMPIMsg() {
  BasicMsg *newMsg = NULL;
  
#ifdef MPI
  struct PriorityRecord {
    int source;
    int tag;
  };
  
  static PriorityRecord priorityList[] = {
    { MPI_ANY_SOURCE, DEBUGMSG },
    { MPI_ANY_SOURCE, MPI_ANY_TAG  }
  };
  
  
  MPI_Status status;
  int i         = 0;
  int msgLength = 0;
  int source    = priorityList[i].source;
  int tag       = priorityList[i].tag;
  int flag      = 0;

  while((flag == 0) && !((source == MPI_ANY_SOURCE) && (tag == MPI_ANY_TAG))) {
    source = priorityList[i].source;
    tag    = priorityList[i].tag;
    
    if(source != MPI_ANY_SOURCE){
      cerr << id << " probing for messages only from " << source << endl;
    }
    
    MPI_Iprobe(source, tag, MPI_COMM_WORLD, &flag, &status);
    
    if(flag) {
      // a message is waiting...  See how long it is
      // Then allocate the needed space for it with new char[]
      // have MPI copy the message into this space
      
      MPI_Get_count(&status, MPI_BYTE, &msgLength);
      newMsg = (BasicMsg *) new char [msgLength];
      MPI_Recv(newMsg, msgLength, MPI_BYTE, source,
	       tag, MPI_COMM_WORLD,&status);
    }
    
    i++;
  }
#endif // MPI
  
  return newMsg;
}

void  
DebugStream::waitForDoneMsgs() {
  BasicMsg* msg = NULL;
  
  while (doneReceivedSoFar < (totalNoOfLps - 1)) {
    if (communicationSystem != NULL) {
      communicationSystem->recvMPI(100);
    }
    else {
      msg = auxRecvMPIMsg();
    }
    
    if (msg != NULL) {
      switch (msg->type) {
      case DEBUGMSG:
	receiveMessage((BasicDebugMsg *) msg);
	delete [] (char *) msg;
	break;
		default:
		break;
      }
    }
  }
}

void
DebugStream::sendDoneMessage() {
  DebugMessage* doneMessage = new DebugMessage;
  
  for(int destLP = 0; (destLP < totalNoOfLps); destLP++) {
    if (destLP != lpId) {
      doneMessage->messageKind = DONE_MESSAGE;
      doneMessage->size        = sizeof(DebugMessage);
      doneMessage->destLP      = destLP;
      remoteSend(doneMessage);
    }
  }
  
  delete doneMessage;
}

DebugStream::~DebugStream() {
  name = NULL;
  if (myKind == CONSOLE) {
    if (lpId == 0) {
      debugFile.close();
    }
    
    sendDoneMessage();

#ifdef MPI
    int errorCode = MPI_SUCCESS;
    char errorString[MPI_MAX_ERROR_STRING];
    int strlength;
    
    if (totalNoOfLps > 1) {
      waitForDoneMsgs();

      errorCode = MPI_Finalize();
      if(errorCode != MPI_SUCCESS) {
	MPI_Error_string(errorCode, errorString, &strlength);
	cout << "MPI_ERROR: " << errorString << endl;
	cout.flush();
      }
    }
#endif
  }
}

void
DebugStream::setLPid(const int lpIdentifier, const int totalLPs, LogicalProcess* lp) {
  lpId         = lpIdentifier;
  totalNoOfLps = totalLPs;
  lpHandle     = lp;

  if (lpId == 0) {
    ostringstream debugFileStream;
    string      debugFileName;

    debugFileStream << "LP" << lpId << ".";
    switch (myKind) {
    case CONSOLE:
      debugFileStream << "console";
      break;
      
    case DEBUG:
      debugFileStream << "debug";
      break;
      
    case ERROR:
      debugFileStream << "error";
      break;
      
    default:
      cerr << "Bad stream kind sepcified." << endl;
      abort();
    }
    
    debugFileStream << ends;
    debugFileName = debugFileStream.str();
    
    debugFile.open(debugFileName.c_str(), ios::out | ios::in);
    
    if (!debugFile.good()) {
      cerr << "Error opening debug file [" << debugFileName << "].\n";
      abort();
    }
    
  }

  if (myKind == CONSOLE) {
    if (lpId == 0) {
      interact();
      sendMessage(-1, RESUME_MESSAGE);
    }
    else {
      pause();
    }
  }
}

void
DebugStream::sendMessage(int dest, const DebugMessageKind msgKind) {
  int destination = (dest == -1) ? 0 : dest;
  DebugMessage *resumeMsg = new DebugMessage;
  
  if (dest == -1) {
    dest = totalNoOfLps;
  }

  resumeMsg->messageKind = msgKind;
  resumeMsg->size        = sizeof(DebugMessage);
  
  while ((destination < totalNoOfLps) && (destination <= dest)) {
    if (destination != lpId) {
      resumeMsg->destLP = destination;
      remoteSend(resumeMsg);
    }
    destination++;
  }
  
  delete resumeMsg;
}

void
DebugStream::sendPartitionInformation() {
  ostringstream partitionStream;
  ObjectRecord* simArray;
  int maxObjectId;
  BasicNoTime* simObject;

  if (lpHandle == NULL) {
    return;
  }

  if (initializeFlag == 2) {
    maxObjectId = lpHandle->getTotalNumberOfObjects();
    simArray    = lpHandle->getSimArray();
    
    for(int objectId = 0; (objectId < maxObjectId); objectId++) {
      if (simArray[objectId].lpNum == lpId) {
	simObject = (BasicNoTime *) simArray[objectId].ptr;
	partitionStream << "  id = " << simObject->id;
	if (simObject->name != NULL) {
	  partitionStream << ", name = " << simObject->name;
	}
	partitionStream << "\n";
      }
    }
  }
  else {
    partitionStream << "Not initialized yet!!!";
  }
  
  partitionStream << ends;

  if (lpId == 0) {
    cout << "Objects on LP0\n";
    cout << partitionStream.str();
    cout << endl;
  }
  else {
    string partitionData = partitionStream.str();
    DebugMessage* partitionMsg = (DebugMessage *) new char[sizeof(DebugMessage) + partitionData.size() + 1];
    new (partitionMsg) DebugMessage();
    memcpy( ((char *) partitionMsg) + sizeof(DebugMessage), partitionData.c_str(), partitionData.size() + 1);
    partitionMsg->size = sizeof(DebugMessage) + partitionData.size() + 1;
    partitionMsg->destLP      = 0;
    partitionMsg->messageKind = PARTITION_MESSAGE;
    remoteSend(partitionMsg);

    delete [] (char *) partitionMsg;
  }
}

void
DebugStream::pause() {
  pauseFlag = false;
  
  if (myKind != CONSOLE) {
    return;
  }
  
  while (pauseFlag == false) {
    if (communicationSystem == NULL) {
      auxRecvMPIMsg();
    }
    else {
      communicationSystem->recvMPI(1);
    }
  }
}
	
void
DebugStream::setid(const int myId) {
  id = myId;
}

void
DebugStream::setCommManager(CommManager* commHandle) {
  communicationSystem = commHandle;
}

void
DebugStream::initialize() {
  if (lpId == 0) {
    if (initializeFlag == 1) {
      initializeFlag = 2;
      interact();
      sendMessage(-1, RESUME_MESSAGE);
    }
  }
  else {
    if (initializeFlag == 1) {
      initializeFlag = 2;
      pause();
    }
  }
}

void
DebugStream::writeMessageToFile(const char *msg) {
  debugFile << msg << endl;
}

void
DebugStream::displayMessage(const char *msg) {
  cout << msg << endl;
}

void
DebugStream::receiveMessage(BasicDebugMsg *basicMsg) {
  DebugMessage* msg = (DebugMessage *) basicMsg;
  char*        data = (((char *) basicMsg) + sizeof(DebugMessage));

#ifdef DEVELOPER_ASSERTIONS
  if (id != 0) {
    cerr << "Error : receiveMessage() called on LP " << id << endl;
  }
#endif
  
  lastMessage = msg->messageKind;

  switch (msg->messageKind) {
  case CONSOLE:
    writeMessageToFile(data);
    displayMessage(data);
    break;
    
  case DEBUG:
    writeMessageToFile(data);
    if (displayMessageFlag == true) {
      displayMessage(data);
    }
    break;
    
  case DONE_MESSAGE:
    doneReceivedSoFar++;
    break;
    
  case RESUME_MESSAGE:
    pauseFlag = true;
    break;

  case PAUSE_MESSAGE:
    pauseFlag = false;
    break;

  case INITIALIZE_MESSAGE:
    initializeFlag = 1;
    break;
    
  case PARTITION_MESSAGE:
    if (lpId == 0) {
      cout << "Objects on LP" << msg->senderLP << endl;
      displayMessage(data);
      partitionInfoCounter++;
    }
    else {
      sendPartitionInformation();
    }
    break;

  case BREAK_POINT_MESSAGE:
    if (lpId == 0) {
      interact();
      sendMessage(-1, RESUME_MESSAGE);
    }
    else {
      pause();
    }
    break;

  case TIME_MESSAGE:
    if (lpId == 0) {
      displayMessage(data);
    }
    else {
      sendTimeInfo(data);
    }
    break;

  default:
    cerr << "Error : Unhandled debug message kind received." << endl;
  }
}

void
DebugStream::executeProcess() {
  /*BasicEvent* debugEvent = */getEvent();
  ostringstream breakMessage;
  breakMessage << "LP" << lpId << " break point at LVT " << getLVT()  
	      << ends;
		  
  string s = breakMessage.str();
  char *c = new char[s.size() + 1];
  memcpy(c, s.c_str(), s.size()+1);
  sendDebugMessage(c);
  
  delete []c;
  sendMessage(-1, BREAK_POINT_MESSAGE);

  if (lpId != 0) {
    pause();
  }
  else {
    interact();
    sendMessage(-1, RESUME_MESSAGE);
  }
}

void
DebugStream::finalize() {
  if (lpId != 0) {
    pause();
  }
  else {
    interact();
    sendMessage(-1, RESUME_MESSAGE);
  }
}

void
DebugStream::sendDebugMessage(const char* message) {
#ifdef MPI  
  DebugMessage* debugMessage = (DebugMessage *) 
    new char[sizeof(DebugMessage) + strlen(message) + 1];;
  new (debugMessage) DebugMessage();

  debugMessage->destLP = 0;
  debugMessage->size   = sizeof(DebugMessage) + strlen(message) + 1;
  debugMessage->messageKind = myKind;

  memcpy(((char *) debugMessage) + sizeof(DebugMessage), message, strlen(message) + 1);
  
  remoteSend(debugMessage);

  delete [] (char *) debugMessage;
#endif
}

void
DebugStream::checkAndSend() {
  if (debugFlag == true) {
    tempDebugStream << ends;
    if (lpId == 0) {
      if (displayMessageFlag == true) {
		string s = tempDebugStream.str();
	    displayMessage(s.c_str());
      }
      if (writeMessageToFileFlag == true) {
		string s = tempDebugStream.str();
		writeMessageToFile(s.c_str());
      }
    }
    else {
		string s = tempDebugStream.str();
		sendDebugMessage(s.c_str());
    }
    tempDebugStream.seekp(0);
  }
}

DebugStream&
operator << (DebugStream& ds, const char *string) {
  ds.tempDebugStream << string;

  return ds;
}

DebugStream&
operator << (DebugStream& ds, const int i) {
  ds.tempDebugStream << i;
  
  return ds;
}

DebugStream&
operator << (DebugStream& ds, const long long time) {
  ds.tempDebugStream << time;

  return ds;
}

DebugStream&
operator << (DebugStream& ds, const float f) {
  ds.tempDebugStream << f;
  
  return ds;
}


DebugStream&
operator << (DebugStream& ds, ostream& (*modifier)(ostream&)) {
/*  if ((modifier == ends) || (modifier == flush) || (modifier == endl)) {
    ds.checkAndSend();
  }*/ // commented by pocho
  return ds;
}

BasicState*
DebugStream::allocateState() {
  return new BasicState;
}

void
DebugStream::setBreakPoint() {
  BasicEvent* breakEvent;
  
  breakEvent = (BasicEvent *) new char[sizeof(BasicEvent)];
  new (breakEvent) BasicEvent();

  cout << "Enter LVT : ";
  cin >> breakEvent->recvTime;
  cout << "Enter LP id : ";
  cin >> breakEvent->dest;
  breakEvent->dest = id - (breakEvent->dest);
  breakEvent->size = sizeof(BasicEvent);
  sendEvent(breakEvent);
  cout << "Break point set.\n";
}

void 
DebugStream::sendTimeInfo(char *objIdStr) {
  ostringstream timeInfo;
  int objId = atoi(objIdStr);
  ObjectRecord* objRec;

  objRec = (lpHandle->getSimArray() + objId);
  if (objRec->lpNum != lpId) {
    timeInfo << "Timing information requested for object not on this LP.";
  }
  else {
    timeInfo << "Time on object ";
    if (objRec->ptr->name != NULL) {
      timeInfo << objRec->ptr->name;
    }
    timeInfo << "(id = " << objId << ") :: ";
    timeInfo << "   lvt = " << objRec->ptr->getLVT();
  }
  
  timeInfo << ends;

  string s = timeInfo.str();
  if (lpId == 0) {
    displayMessage(s.c_str());
  }
  else {
    DebugMessage* timeMsg = (DebugMessage *) new char[sizeof(DebugMessage) + 
						     s.size() + 1];
    new (timeMsg) DebugMessage();
    timeMsg->messageKind = TIME_MESSAGE;
    timeMsg->size        = sizeof(DebugMessage) + s.size() + 1;
    timeMsg->destLP      = 0;
    memcpy(((char *) timeMsg) + sizeof(DebugMessage), s.c_str(), s.size() + 1);
    remoteSend(timeMsg);
    delete [] (char *) timeMsg;
  }
  
}

void
DebugStream::findTimeForObject(int objId) {
  char buffer[80];
  ObjectRecord* objRec;

  sprintf(buffer, "%d", objId);
  objRec = (lpHandle->getSimArray() + objId);
  
  if (objRec->lpNum == 0) {
    sendTimeInfo(buffer);
  }
  else {
    DebugMessage* timeMsg = (DebugMessage *) new char[sizeof(DebugMessage) + strlen(buffer) + 1];
    new (timeMsg) DebugMessage();
    
    timeMsg->destLP = objRec->lpNum;
    timeMsg->size   = sizeof(DebugMessage) + strlen(buffer) + 1;
    timeMsg->messageKind = TIME_MESSAGE;
    memcpy(((char *) timeMsg) + sizeof(DebugMessage), buffer, strlen(buffer) + 1);
    remoteSend(timeMsg);
    
    delete [] (char *) timeMsg;
    
    lastMessage = NO_MESSAGE;
    while (lastMessage != TIME_MESSAGE) {
      communicationSystem->recvMPI(1);
    }
  }  
}

void
DebugStream::findTime() {
  int  objId;

  cout << "Enter object id [-1 for all] : ";
  cin >> objId;

  if (objId == -1) {
    for(objId = 0; (objId <= (id - totalNoOfLps + 1)); objId++) {
      findTimeForObject(objId);
    }
  }
  else {
    if ((objId < 0) || (objId > (id - totalNoOfLps + 1))) {
      cout << "Bad object id.\n";
      return;
    }
    findTimeForObject(objId);
  }
}

void
DebugStream::interact() {
  char buffer[30] = "";

  if (lpId != 0) {
    return;
  }
  
  do {
    communicationSystem->recvMPI(1000);
    cout << endl << PROMPT << flush;
    char *result = gets(buffer);
	if (result == 0)
	{ 
		/* gets failed */
	}
    
    switch (buffer[0]) {
    case 'D':
    case 'd':
      displayMessageFlag = ((displayMessageFlag == false) ? true : false);
      cout << "Debug " << ((displayMessageFlag == false) ? "off" : "on")
	   << endl;
      break;
      
    case 'b':
    case 'B':
      if (initializeFlag == 2) {
	setBreakPoint();
      }
      else {
	cout << "You have to initialize first." << endl;
      }
      break;
      
    case 'h':
    case 'H':
      cout << "H - Help" << endl
	   << "D - Debug on/off" << endl
	   << "B - Set break point" << endl
	   << "C - Continue" << endl
	   << "I - Initialize simulation objects" << endl
	   << "P - Partition information" << endl
	   << "T - Time" << endl;
      break;

    case 'C':
    case 'c':
      return;

    case 'I':
    case 'i':
      switch(initializeFlag) {
      case 0: 
	sendMessage(-1, INITIALIZE_MESSAGE);
	initializeFlag = 1;
	return;
      case 2:
	cout << "Initialization is already complete.\n";
	break;
      }
      break;
      
    case 'p':
    case 'P':
      if (initializeFlag == 2) {
	sendPartitionInformation();
	partitionInfoCounter = 0;
	sendMessage(-1, PARTITION_MESSAGE);
	while (partitionInfoCounter < (totalNoOfLps - 1)) {
	  communicationSystem->recvMPI(1000);
	}
      }
      else {
	cout << "Objects have to be initialized first." << endl;
      }
      break;
      
    case 't':
    case 'T':
      if (initializeFlag == 2) {
	findTime();
      }
      else {
	cout << "Objects have to be initialized first." << endl;
      }
      break;

    default:
      cout << "Sorry! unkown command.\n";
      break;
    } 
  } while (1);
}

#endif

