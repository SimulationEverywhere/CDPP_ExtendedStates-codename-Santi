/*******************************************************************
*
*  DESCRIPTION: definitions of messages ( Y, *,@,  D, X, I )for the TimeWarp version
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar

*
*  DATE: 02/11/2000
*
*******************************************************************/

#include "pmessage.h"
#include "model.h"
#include "pmodeladm.h"

using namespace std;

InitMessage* TWInitMessage::getMessage() {

	//create a new init message with the receive time and the destination.
	return (new InitMessage( recvTime, procId));
}

InternalMessage* TWInternalMessage::getMessage() {

	//create a new Internal Message
	return (new InternalMessage( recvTime, procId));

};	// class TWInternalMessage


OutputSyncMessage* TWOutputSyncMessage::getMessage() {

	//create a new Internal Message
	return (new OutputSyncMessage( recvTime, procId));

};	// class TWOutputSyncMessage

CollectMessage* TWCollectMessage::getMessage() {

	//create a new Internal Message
	return (new CollectMessage( recvTime, procId));

};	// class TWCollectMessage


DoneMessage* TWDoneMessage::getMessage() {

	//create a new Done Message
	return (new DoneMessage(recvTime, procId, nextChange, slaveSync));

}


BasicExternalMessage* TWExternalMessage::getMessage() {

	BasicMsgValue *value = BasicMsgValue::build_from(*this->val);

	//Make a copy of the current BasicMsgValue
	//value = clone();

	//Find the port where the message is being received...
	Model &model ( SingleParallelModelAdm::Instance().model(mid));
	Port &port ( model.port(p) );
	return (new BasicExternalMessage(recvTime, procId, port, value, sendermid));

}

BasicOutputMessage* TWOutputMessage::getMessage() {

	BasicMsgValue *value = BasicMsgValue::build_from(*this->val);

	//Make a copy of the current BasicMsgValue
	//value = clone();

	//Find the port where the message is being sent to...

	Model &model ( SingleParallelModelAdm::Instance().model(mid));
	Port &port ( model.port(p) );
	return (new BasicOutputMessage(recvTime, procId, port, value));

}
