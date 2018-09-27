#ifndef COMM_MANAGER_CC
#define COMM_MANAGER_CC

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

#include "CommManager.h"
#include "LogicalProcess.h"
#include "NoTime.h"
#include "KernelMsgs.h"
#include <string>
#include "DebugStream.h"
#include <cstring>

#define BUFFER_SIZE 2097152

using namespace std;


CommManager::CommManager(int numberOfLps, LogicalProcess* lp) : BasicNoTime() {
	numLPs = numberOfLps;
	id     = 0;
	setLPHandle(lp);

	terminateTokenFlag   = 0;
	terminateTokenNumber = 0;
}  

void 
CommManager::executeProcess(){
	cerr << "Error!  CommManager::executeProcess() called" << endl;
}

BasicEvent* 
CommManager::getEvent() {
	cerr << "Error! CommManager::getEvent() called." << endl;

	return NULL;
}

CommManager::~CommManager(){
#if ((defined MPI) && !(defined INTERACTIVE))
	int errorCode;
	char errorString[MPI_MAX_ERROR_STRING];
	int strlength;

	if( numLPs > 1 ){
		errorCode = MPI_Finalize();
		if(errorCode != MPI_SUCCESS) {
			MPI_Error_string(errorCode, errorString, &strlength);
			cout << "MPI_ERROR: " << errorString << endl;
			cout.flush();
		}
	}
#endif
	delete [] sendSequenceArray;
	delete [] recvSequenceArray;
}

void
CommManager::waitForInit(int numExpected) {
	int numReceived = 0;

	while(numReceived != numExpected ) {
		BasicMsg * msg = recvMPIMsg();
		if((msg != NULL) && (msg->type == INITMSG)) {
			numReceived++;
			routeMsg((BasicMsg *) msg);
		}
		else if ( msg != NULL ) {
      // we didn't find an init msg, but we need to check for other msgs
			routeMsg((BasicMsg *)&msg);
		} 
	} 
}

void
CommManager::waitForStart()  {
	BasicMsg *msg = NULL;
	bool finished = false;

	while(finished == false)  {
		msg = recvMPIMsg();
		if((msg != NULL) && (msg->type == STARTMSG))  {
			finished = true;
			recvSequenceArray[msg->senderLP]++;
			delete [] ((char *) msg);
		} 
		else if(msg != NULL) {
      // we didn't find an init msg, but we need to check for other msgs
			routeMsg((BasicMsg *) msg);
		} 
	}
}

void
CommManager::initCommManager(ObjectRecord* sim, int lpNum) { 
	if (lpNum > 1) {
#ifdef MPI
		MPI_Comm_rank(MPI_COMM_WORLD, &id); 
		static char buffer[BUFFER_SIZE];
		MPI_Buffer_attach((void*)buffer,BUFFER_SIZE);
#endif
	}

	communicationHandle = sim;

	sendSequenceArray = new SequenceCounter[numLPs];
	recvSequenceArray = new SequenceCounter[numLPs];

	for(int i = 0; i < numLPs; i++){
		sendSequenceArray[i] = 1;
		recvSequenceArray[i] = 0;
	}
	if (id == 0) {
		circulateToken = true;
	} else {
		circulateToken = false;
	}
}

void
CommManager::recvEvent(BasicEvent* event)  {
	EventMsg* mpiMsg;
	int msgSize = sizeof(BasicMsg) + event->size;

	mpiMsg = (EventMsg*) new char [msgSize];
	memcpy((BasicEvent *) mpiMsg, event, event->size);

	mpiMsg->destLP   = communicationHandle[event->dest].lpNum;
	mpiMsg->senderLP = id;
	mpiMsg->type     = EVENTMSG;
	mpiMsg->size     = event->size;

	int dest = mpiMsg->destLP;
	mpiMsg->senderLP = id;
	mpiMsg->sequence = sendSequenceArray[dest];
	sendSequenceArray[dest]++;

	if( mpiMsg->destLP == id ) {
		cerr << "Warning : CommManager " << id 
			<< " received an event for a local guy : " << *event << endl;
		deliver( mpiMsg );
		delete [] (char* ) mpiMsg;
	}
	else  {
		remoteSend(mpiMsg, msgSize);
		delete [] (char *) mpiMsg;
		delete [] (char *) event;
	}
}

void
CommManager::recvMsg(TerminateMsg *msg) {
  // uh oh...  it's a terminate message... we'd better
  // generate messages for all the other guys
	msg->senderLP = id;

	for(int i = 0; (i < numLPs); i++)  {
		msg->sequence = sendSequenceArray[i];
		sendSequenceArray[i]++;

		msg->destLP = i;

		if(i != id) {
			remoteSend(msg, sizeof(TerminateMsg));
		}
	}

  // Now we need to handle the termination... Let's call the method that
  // handles this
	terminateSimulation(msg);
}

void
CommManager::recvMsg(DummyMsg *dummy)  {
	dummy->senderLP = id;
	dummy->sequence = sendSequenceArray[dummy->destLP];
	sendSequenceArray[dummy->destLP]++;

	remoteSend((BasicMsg *)dummy, sizeof(DummyMsg));
}

void
CommManager::recvMsg(InitMsg * msg) {
	msg->senderLP = id;
	msg->sequence = sendSequenceArray[msg->destLP];
	sendSequenceArray[msg->destLP]++;

	remoteSend((BasicMsg *)msg, sizeof(InitMsg));
}

void
CommManager::recvMsg(StartMsg* msg) {
	msg->senderLP = id;
	msg->sequence = sendSequenceArray[msg->destLP];
	sendSequenceArray[msg->destLP]++;

	remoteSend((BasicMsg *)msg, sizeof(StartMsg));
}

void
CommManager::recvMsg(CheckIdleMsg* msg) {
	msg->senderLP = id;
	msg->sequence = sendSequenceArray[msg->destLP]++;

	remoteSend((BasicMsg *) msg, sizeof(CheckIdleMsg));
}

void
CommManager::recvMsg(BasicDebugMsg* msg) {
	msg->senderLP = id;
	msg->sequence = sendSequenceArray[msg->destLP]++;

	remoteSend((BasicMsg *) msg, msg->size);
}

void
CommManager::deliver(BasicEvent* event)  {
	communicationHandle[event->dest].ptr->recvEvent(event);
}  

void
CommManager::deliver(BasicDebugMsg* msg) {
#ifdef INTERACTIVE
	console.receiveMessage(msg);
#endif
}

void
CommManager::routeMsg(BasicMsg* msg) {
	switch(msg->type) {
		case INITMSG:
			{
				InitMsg* initMsg = (InitMsg *) msg;
      // we just got an init msg...  someone needs to update our simArray...
      // let's do it right here for now...
      // first let's make sure WE don't have that object!!

				int objId = initMsg->objId;
				if((communicationHandle[objId].lpNum == id) && (initMsg->lpId != id))  {
	// we've already registered this guy locally!
					cerr << "CommManger " << id << " received init msg : " << *initMsg
						<< " and it has a record of object " << objId
						<< " being local!" << endl;
				}
				else{
					communicationHandle[objId].lpNum = initMsg->lpId;
	// point the pointer at the CommManager, so it takes care of the
	// communications	
					if(communicationHandle[objId].lpNum!=id){
						communicationHandle[objId].ptr = this;
					}
				} // the object isn't already local...

      // now we can delete it!
				delete [] ((char *) initMsg);
				break;
			}

		case STARTMSG:
				cerr << "CommManager " << id << " asked to route a start message!" << endl;
			delete [] ((char *) msg);
			break;

		case EVENTMSG: 
			{
				EventMsg* eventMsg = (EventMsg *) msg;
				terminateTokenFlag = 0;

      // first, we need to get the size of the BasicEvent contained in this
      // message, and allocate a new space for it...
				if(eventMsg->size <= 0)  {
					cerr << "LP " << id << ": Bogus message received from MPI" << endl;
					cerr << "eventMsg has size = " << eventMsg->size << endl;
					abort();
				}

				BasicEvent *toDeliver = (BasicEvent *) new char[eventMsg->size];  
				memcpy(toDeliver, (BasicEvent *) eventMsg, eventMsg->size);
				delete [] ((char *) msg);
				deliver(toDeliver);
				break;
			}

		case TERMINATEMSG:
				terminateSimulation(new TerminateMsg);
			break;

		case DUMMYMSG:
				deliver((DummyMsg *) msg);
			break;

		case CHECKIDLEMSG: {

				CheckIdleMsg *recvdMsg   = (CheckIdleMsg *) msg;
				if (getLPHandle()->isIdle() == true) {
					if (terminateTokenNumber == recvdMsg->tokenNum) {
	// This means the same token has already reached this LP
						if (terminateTokenFlag == 0) {
							terminateTokenFlag = 2; // terminate this LP
						}
					} else {
						terminateTokenFlag = 0;
						terminateTokenNumber = recvdMsg->tokenNum;
					}
					circulateToken = false;

					if (terminateTokenFlag != 3) {
						CheckIdleMsg *idleMsg    = new CheckIdleMsg;
						idleMsg->tokenNum        = terminateTokenNumber;
						idleMsg->destLP          = ((id == (numLPs - 1)) ? 0 : (id + 1));
						recvMsg(idleMsg);
						delete idleMsg;
						if (terminateTokenFlag == 1) {
	  // This LP began circulating the token so make this LP terminate it
							terminateTokenFlag = 3;
						}
					} else {
	// This would stop circulating tokens and terminate the last LP 
						terminateTokenFlag = 2;
					}
				} else {
      // This LP has to start circulating the token in future
					terminateTokenFlag = 0;
					circulateToken = true; 
      // Give a fresh token number for the next circulation
					terminateTokenNumber = recvdMsg->tokenNum;
				}

				delete [] ((char *) msg);
				break;
			}
		case DEBUGMSG:
#ifdef INTERACTIVE
				console.receiveMessage((BasicDebugMsg *) msg);
#endif
			delete [] (char *) msg;
			break;

		default:
			cout << "CommManager " << id << ": unknown message (" << msg << ") type "
				<< *msg << " received!" << endl;
			TerminateMsg* ouch = new TerminateMsg;
			strcpy(ouch->error, "unknown message type found...");
			recvMsg(ouch);
			break;
	}
}

int
CommManager::recvMPI(int maxNum) {
	int numReceived      = 0;
	bool noMessagesLeft = false;
	BasicMsg * msgReceived;

	while((numReceived != maxNum) && (noMessagesLeft == false)) {
		msgReceived = recvMPIMsg();
		if(msgReceived == NULL) {
			noMessagesLeft = true;
		}
		else {
			numReceived++;
			recvSequenceArray[msgReceived->senderLP]++;
			routeMsg(msgReceived);
		}
	}

	return numReceived;
}

BasicMsg *
CommManager::recvMPIMsg()  {
	BasicMsg *newMsg = NULL;

#ifdef MPI
	struct PriorityRecord {
		int source;
		int tag;
	};

	static PriorityRecord priorityList[] = {
		{ MPI_ANY_SOURCE, DEBUGMSG },
		{ MPI_ANY_SOURCE, TERMINATEMSG },
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
CommManager::deliver(DummyMsg *dummy) {}

void
CommManager::remoteSend(BasicMsg* msg, int size) {
#ifdef MPI  
	MPI_Bsend(msg, size, MPI_BYTE, msg->destLP,
		msg->type, MPI_COMM_WORLD);
#endif
}

SequenceCounter
CommManager::getSequence(int lpId) {
	return sendSequenceArray[lpId];
}

void
CommManager::terminateSimulation(TerminateMsg* ouch) {
	cerr << "CommManager " << id << " received terminate message :\n"
		<< *ouch << endl;

	exit(-1);
}

void 
CommManager::sendEvent(BasicEvent* event)  {
	cerr << "sendEvent( BasicEvent* ) called in CommManager "
		<< id << " : " << *event << endl;
}

void
CommManager::iHaveObject(int objId)  {
	InitMsg* msg = new InitMsg;
	msg->objId   = objId;

	msg->lpId   = id;
	msg->destLP = 0;

	recvMsg(msg);

	delete msg;
}

bool
CommManager::isSimulationComplete() {
	if (numLPs == 1) {
		return true;
	}

	switch(terminateTokenFlag) {
		case 2:
			return true;
			break;
		case 0:
			if (circulateToken) {
				CheckIdleMsg* idleMsg    = new CheckIdleMsg;
				idleMsg->tokenNum        = ++terminateTokenNumber;
				idleMsg->destLP          = ((id == (numLPs - 1)) ? 0 : (id + 1));
				terminateTokenFlag       = 1;
				circulateToken           = false;

				recvMsg(idleMsg);
				delete idleMsg;
				cout << "LP " << id << " sending token " << idleMsg->tokenNum << endl;
			}
			return false;
		default:
			return false;
	}
}

#ifdef MPI
void
physicalCommInit(int *argc, char ***argv){
	MPI_Init(argc, argv);
}

int
physicalCommGetId(){
	int id;
	MPI_Comm_rank(MPI_COMM_WORLD, &id);
	return id;
	
}
#endif

#endif

