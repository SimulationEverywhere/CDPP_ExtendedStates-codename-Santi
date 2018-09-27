/*******************************************************************
*
*  DESCRIPTION: class MacroExpansion
*
*  AUTHOR: Daniel A. Rodriguez 
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 25/7/1999 (v2)
*
*******************************************************************/

#ifndef __MACROEXPANSION_H
#define __MACROEXPANSION_H

/** include files **/
#include <list>
#include <cstdlib>
#include <string>
#include "except.h"

/** declarations **/
class macroExpansion
{
public:
	macroExpansion(std::string fileName);	// Constructor

	~macroExpansion();			// Destructor
	
	std::string expand();			// Make the Macro Expansion

	std::string tempFileName()			// Devuelve el nombre del
		{return newFileName;}		// archivo temporario creado
						// por Expand.	
private:
	int isInclude( char *line );
	int isInclude( char *line, long posLine );

	int isMacro( char *line )
			{ return isMacro(line, 0); }
			
	int isMacro( char *line, long posLine );
	int isEndOfMacro( char *line );
	int isBeginOfMacro( char *line );
	
	std::string getData( char *line, std::string macroCmdName )
			{ return getData( line, 0, macroCmdName); }

	std::string getData( char *line, long posLine, std::string macroCmdName );
	
	void addIncludeFile( std::string file );
	void putMacroExpansion( FILE *fileOut, std::string macroName );
	int  putMacroExpansionInFile( FILE *fileOut, std::string macroName, std::string macroFile );
	int  copyMacro( FILE *fileOut, FILE *fileIn, char *macroLine, std::string macroName);

	///////////////////////
	// Internal structure
	///////////////////////
	std::string		fileName;
	std::string		newFileName;
	std::list<std::string>	includeFiles;
	
}; // macroExpansion


extern macroExpansion	*instanceMacroExpansion;

inline macroExpansion &MacroExpansion(std::string fileName = "")
{
	if (instanceMacroExpansion == NULL)
		instanceMacroExpansion = new macroExpansion(fileName);
		
	return *instanceMacroExpansion;
}


#endif	// macroExpansion
