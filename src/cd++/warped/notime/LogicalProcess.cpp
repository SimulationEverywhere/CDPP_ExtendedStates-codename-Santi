#ifndef LOGICAL_PROCESS_CC
#define LOGICAL_PROCESS_CC

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
#include "SimulationTime.h"
#include <math.h>
#include <iomanip>
#include <sstream>
#include <string>
#include <cstring>
#include "Interactive.h"

#ifdef INTERACTIVE
#include "DebugStream.h"
#endif

using namespace std;

LogicalProcess::LogicalProcess(int totalNum, int myNum, int lpNum) : comm(lpNum, this) {
#ifdef INTERACTIVE
  // Account for the new console process too...
  totalNum          += lpNum;
  myNum++;
#endif

  totalObjects       = totalNum;
  numObjects         = myNum;
  numLPs             = lpNum;
  simulationFinished = false;
  initialized        = false;
  numRegistered      = 0;
  simArray           = new ObjectRecord[totalObjects];
  
  comm.initCommManager(simArray, numLPs);  
  id = comm.getID();
  
  for (int i = 0; (i < totalObjects); i++) {
    simArray[i].ptr   = NULL;
    simArray[i].lpNum = -1;
  }

#ifndef MPI
  if((numLPs > 1) && (id == 0))  {
    cerr << "Error! numLPs = " << numLPs 
	 << " but no MPI support compiled in!\n";
  }
#endif

#ifdef INTERACTIVE  
  // The order is kinda imporatant here
  console.setCommManager(&comm);
  console.setid(totalNum - 1 - id);
  console.setLPid(id, lpNum, this);
#endif
}

LogicalProcess::~LogicalProcess() {
#ifdef INTERACTIVE
  console.setCommManager(NULL);
#endif
  cout << "LP " << id << " has finished simulation \n";
  delete [] simArray;
}

void 
LogicalProcess::simulate(VTime) {
  BasicEvent *eventToProcess = NULL;
#ifdef MPI
  int startTerminateCounter = 0, count = 0, messRecv = 0;
  int infreqMsgsGot = 0, infreqSkips = 0, infreqCurFreq = 3;
  int pollDelta = 4, pollMax = 25, idleCnt = 0;
#endif

  if(initialized == false) {
    cerr << "allRegistered not called in LP " << id << endl;
  }
  
  // wait for init messages for every object in the system, except
  // for our own...
  
  if(id == 0) {
    StartMsg foo;
    // let's send out start messages - we don't need one ourselves...
    for(int i = 1; (i < numLPs); i++) {
      foo.destLP = i;
      comm.recvMsg(&foo);
    }
  }
  else {
    comm.waitForStart();
  }
  
  console << "LP " << id << ": starting simulation (NoTime)." << endl;

#ifdef MPI
  simulationFinished = false;
#else
  simulationFinished = ((inputQ.getHead() != NULL) ? false : true);
#endif
  
  while(simulationFinished == false) {
#ifdef MPI
    if (numLPs > 1) {
#ifdef INFREQ_POLLING
      if (infreqSkips == 0) {
	infreqMsgsGot = comm.recvMPI(1000);
	count++;
	messRecv += infreqMsgsGot;
	if (infreqMsgsGot > 0) {
	  if(infreqMsgsGot > 1) {
	    infreqCurFreq -= pollDelta ;
	    if(infreqCurFreq < 0) infreqCurFreq = 0 ;
	  }
	} else {
	  // we got zero messages , let's increase poll freq
	  infreqCurFreq += pollDelta ;
	  if(infreqCurFreq > pollMax) infreqCurFreq = pollMax ; 
	}
	infreqSkips = infreqCurFreq;
      } else {
	infreqSkips--;
      }
#else // !INFREQ_POLLING
      infreqMsgsGot = comm.recvMPI(1000);
#endif
    }
#endif

    if ((eventToProcess = inputQ.getHead()) != NULL) {
#ifdef MPI
      startTerminateCounter = 0;
#endif
      simArray[eventToProcess->dest].ptr->executeSimulation();
      inputQ.removeAlreadyProcessedEvents();
    }
    else {
#ifdef MPI
      if (++startTerminateCounter > 1000) {
	simulationFinished = comm.isSimulationComplete();
      }
#else      
      simulationFinished = true;
#endif      
    }
  }
  
  for(int i = 0; (i < totalObjects); i++) {
    simArray[i].ptr->finalize();
  }
  
  if(id == 0) {
    console << "Simulation complete!" << endl;
  }
}

void 
LogicalProcess::registerObject(BasicNoTime* object) {
  numRegistered++;
  
  if ((object->id >= totalObjects) || (object->id < 0)) {
    ostringstream errmsg;
	string s;
	
    errmsg << "Object id " << object->id << " trying to register, but " 
	   << totalObjects-1 << " is the largest id allowed!" << ends;
	s = errmsg.str();
    
    TerminateMsg * ouch = new TerminateMsg;
    strcpy(ouch->error, s.c_str());
    comm.recvMsg(ouch);
  }
  
  if (simArray[object->id].ptr == NULL)  {
    simArray[object->id].ptr   = object;
    simArray[object->id].lpNum = id;
    
    object->setLPHandle(this);
    object->setCommHandle(simArray);
    object->setInputQueue(&inputQ);
  }
  else {
    cerr << "Object " << object->id << " trying to register twice!" << endl;
    TerminateMsg * ouch = new TerminateMsg;
    strcpy(ouch->error, "Object tried to register twice" );
    comm.recvMsg(ouch);
  }
}

void
LogicalProcess::allRegistered( void ) {

#ifdef INTERACTIVE
  registerObject(&console);
#endif
  
  if (numRegistered == numObjects) {
    if(id == 0){
      // at this point, if we are object 0, we should wait for init messages
      // from everyone...
      
      comm.waitForInit(totalObjects - numObjects);
      
      // now we've got all of the objects... we need to send out all of the
      // objects to every one else
      InitMsg initMsg;
      for(int i = 0; (i < totalObjects); i++) {
	initMsg.objId = i;
	initMsg.lpId = simArray[i].lpNum;
	for(int j = 1; (j < numLPs); j++) {
	  initMsg.destLP = j;
	  comm.recvMsg(&initMsg);
	}
      }
    }
    else {
      // we should walk through our local array sending the objects to the
      // main guy...
      for(int i = 0; (i < totalObjects); i++)  {
	if (simArray[i].lpNum == id) {
	  comm.iHaveObject(simArray[i].ptr->id);
	}
      }
      
      // OK, now we need to receive all of the data FROM the head guy...
      comm.waitForInit( totalObjects );
    }
    
    console << "LP " << id << ": initializing simulation objects" << endl;
    
    // call the initialization routine and save initial state for everyone
    for(int i = 0; (i < totalObjects); i++) {
      if (simArray[i].lpNum == id) {
	simArray[i].ptr->timeWarpInit();
      }
    }
    initialized = true;
  }
  else {
    cerr << "LP " << id << " incorrect number of objects registered!" << endl;
    cerr << "Expected " << numObjects << " objects, and " << numRegistered <<
      " registered!" << endl;
    TerminateMsg * ouch = new TerminateMsg;
    strcpy(ouch->error, "invalid number of objects registered" );
    comm.recvMsg(ouch);   
  }
}

int
LogicalProcess::getNumObjects() {
  return numObjects;
}

int
LogicalProcess::getTotalNumberOfObjects() const {
  return totalObjects;
  
}

bool
LogicalProcess::isIdle() const {
  return ((inputQ.getHead() != NULL) ? false : true);
}

#endif

