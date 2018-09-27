#ifndef LOGICAL_PROCESS_HH
#define LOGICAL_PROCESS_HH

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

#include "BasicEvent.h"
#include "CommManager.h"
#include "DefaultVTime.h"
#include "BasicNoTime.h"
#include "ObjectRecord.h"
#include "InputQueue.h"
#include "Interactive.h"

#ifdef __OLD_C
extern "C" {
  long lrand48();
  void srand48(long int);
}
#endif

class LogicalProcess {
  friend class BasicNoTime;
public:
  // Arguments are: Total number of Objects, number of objects in this LP,
  // number of LPs, and time upto which to simulate.
  LogicalProcess(int, int, int);
  ~LogicalProcess();
  
  int getNumObjects();
  int getTotalNumberOfObjects() const;
  
  int getLPid(){
    return id;
  }
  
  void simulate(VTime simUntil = PINFINITY);

  void allRegistered(void); // called by application to tell the kernel 
                            // when all of the simulation objects in this
                            // LP have registered
  
  void registerObject(BasicNoTime*);

  bool isIdle() const;
  
  ObjectRecord* getSimArray() {
    return simArray;
  }

  // Time upto which to simulate.
  // The definition of this variable is in main.cc
  static const VTime SIMUNTIL;

#ifdef DEBUG
  std::ostream* getDebugFile() const {
    return debugFile;
  }
#endif
  
protected:
  InputQueue inputQ;

#ifdef DEBUG
  ofstream *debugFile;
#endif

private:
  bool initialized;     // flag that gets set if we've been initialized
  bool simulationFinished;
  
  int numRegistered;
  int numObjects;       // number of objects on this LP
  int totalObjects;     // total number of sim objects
  int numLPs;           // the number of LPs
  int id;               // my LP id
  
  ObjectRecord* simArray;

  CommManager comm;     // our walkie talkie to the other LPs
};

#endif
