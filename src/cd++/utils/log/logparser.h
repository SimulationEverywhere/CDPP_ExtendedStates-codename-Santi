/*******************************************************************
*
*  DESCRIPTION: Utilities to parse the simulation log
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 19/04/2001
*
*******************************************************************/
#ifndef __LOGPARSER_H_
#define __LOGPARSER_H_

#include "VTime.h"    // Class VTime
#include <strstream>
#include "strutil.h" // str2Int y str2Real
#include "cellpos.h"

//For the draw log utility.
bool parseLine( const char *l, VTime &current, const std::string &modelName,
                CellPosition &posi, Real &value, const std::string &portName  );

bool isMessageLine( const char* l, int& LP, VTime &msgTime);

bool isMessageLine( const char* l, int& LP, VTime &msgTime, std::string& msgType);




#endif //LOGPARSER_H
