/*******************************************************************
*
*  DESCRIPTION: CLASS StandAloneLoader
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 11/4/1999 (v2)
*  DATE: 29/8/2000 (v3)
*
*******************************************************************/

#ifndef __STDALOAD_H
#define __STDALOAD_H

/** include files **/
#include "loader.h"
#include "macroexp.h"

/** declarations **/

class StandAloneLoader : public SimLoader
{
public:
	StandAloneLoader(int argc, char** argv); 	
	SimLoader& openComm();
	SimLoader& closeComm();
	SimLoader &loadData();
	
	~StandAloneLoader();	 //Destructor
	
protected:
	
	SimLoader &writeResults();
	
private:
	std::string	iniName;
	std::string	evName;
	std::string	evalDebugName;
	std::string	flatDebugName;
	std::string	parserFileName;
	int	preprocessor;	// if is 0 then bypass the preprocessor
	std::string	partitionFileName;	
	
	void readCommandLine();
	
};	// class StandAloneLoader

#endif   //__STDALOAD_H 
