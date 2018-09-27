#ifndef COMM_MANAGER_HH
#define COMM_MANAGER_HH

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
//
//---------------------------------------------------------------------------

#include "BasicNoTime.h"
#include "KernelMsgs.h"

class LogicalProcess;
class ObjectRecord;

#ifdef MPI
extern "C" {
#include "mpi.h"
}
#endif

void physicalCommInit(int *argc, char ***argv);
int physicalCommGetId();

class CommManager : public BasicNoTime {
public:
	CommManager(int numberOfLPs, LogicalProcess*);
	~CommManager();

	void initCommManager(ObjectRecord*, int);

	int getID() {  return id; };

	bool isCommManager() {
		return true;
	}

	bool isSimulationComplete();

  // These methods are for the _Commucation Manager_ to receive the types
  // of events passed in ( by local objects )

	void recvEvent(BasicEvent*);

	void recvMsg(TerminateMsg *);
	void recvMsg(InitMsg *);
	void recvMsg(StartMsg *);
	void recvMsg(DummyMsg *);
	void recvMsg(CheckIdleMsg *);
	void recvMsg(BasicDebugMsg *);

	void deliver(BasicEvent *);
	void deliver(DummyMsg *);
	void deliver(BasicDebugMsg *);

  // this is a method the LP calls to let everyone know that he's got a
  // certain simulation object in his posession.
	void iHaveObject(int);

  // wait for the number of messages specified (and deliver them).
  // If there aren't that many messages, stop receiving them.
  // In either case, return the number of messages actually received.
	int recvMPI(int = 1);

  // grab ONE message from MPI and return it..
  // return NULL if there are none...
	BasicMsg * recvMPIMsg();

	void terminateSimulation( TerminateMsg * );

  // returns the sequence number of the next message to the LP with the id
  // passed in
	SequenceCounter getSequence( int );

	void waitForStart();
	void waitForInit( int numExpected );

	void sendEvent( BasicEvent*);

  // We need to define these to make the compiler happy.
	virtual void executeProcess();
	BasicEvent* getEvent();

private:

	int numLPs;
	int id;

	int terminateTokenFlag;
	int terminateTokenNumber;
	bool circulateToken;

  // these will be filled in with arrays so that each communication channel
  // has a unique sequence running

	SequenceCounter* sendSequenceArray;
	SequenceCounter* recvSequenceArray;

	void remoteSend( BasicMsg*, int );

  // this method takes an LP Msg, figures out what kind of derived type it is,
  // and then delivers it to the LOCAL entity that is receiving it.
	void routeMsg( BasicMsg *);
};


#endif
