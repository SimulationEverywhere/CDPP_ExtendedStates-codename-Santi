/*******************************************************************
*
*  DESCRIPTION: class macroExpansion
*
*  AUTHOR: Daniel A. Rodriguez
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*
*  DATE: 25/07/1999 (v2)
*
*******************************************************************/

/** include files **/
#include <unistd.h>
#include <cstdlib>
#include "macroexp.h"	// class macroExpansion
#include "strutil.h"	// string utils
#include <cstring>

#define TAM_BUFFER	32000

#define MACRO_HEADER		'#'
#define COMENTARIO_HEADER	'%'
#define MACRO_INCLUDE		"#INCLUDE"
#define MACRO_MACRO		"#MACRO"
#define MACRO_BEGINMACRO	"#BEGINMACRO"
#define MACRO_ENDMACRO		"#ENDMACRO"


#ifdef __WINDOWS__			// Ugh!!!
	#define	SEPARATOR		'\\'
	#define	SEPARATOR_STR		"\\"
	#define	SEPARATOR_HEADER	".\\"
#else					// We're in Unix !!!!!!!
	#define	SEPARATOR		'/'
	#define	SEPARATOR_STR		"/"
	#define	SEPARATOR_HEADER	"./"
#endif


using namespace std;

macroExpansion	*instanceMacroExpansion = NULL;


macroExpansion::macroExpansion(string file)
:fileName(file)
,newFileName("")
{
	MASSERTMSG( file != "", "Can't create a MacroExpansion with an invalid filename.\n");
}

char *advanceWhileChar( char *line, char c )
{
	while( *line != 0 && *line == c)
		line++;

	return line;
}

int isStringSimilar( char *line, long posLine, char *cadena )
{
	line = advanceWhileChar( line + posLine, ' ' );

	if (*line == 0)
		return 0;

	string	sAux(line);

	if (upcase(substringLenght(sAux, strlen(cadena))) == cadena)
		return 1;
	return 0;
}

inline int isStringSimilar( char *line, char *cadena )
{
	return isStringSimilar(line, 0, cadena);
}

long advanceMacro(char *line, long posLine)
{
	while ( *(line + posLine) != ')' )
		posLine++;

	return posLine;
}

macroExpansion::~macroExpansion()
{
	if (!newFileName.empty())
		unlink(newFileName.c_str());
	
	// Hack to avoid deleting twice the same object.	
	instanceMacroExpansion = NULL;
}

int isInTheRootDirectory(const char *fileName, char separator)
{
	register long	cant = 0;

	while (*fileName != 0)
	{
		if (*fileName == separator)
			cant++;

		fileName++;
	}
	return cant == 1;
}

string getFileName(const char *pathName)
{
	// Search for the begginig of the fileName
	int	posi = strlen(pathName) - 1;

	while (pathName[posi] != SEPARATOR)
		posi--;

	// Found the separator
	string	sAux = SEPARATOR_STR;

	posi++;
	while (pathName[posi] != 0)
		sAux += pathName[posi++];

	return sAux;
}

void includePathEnvironment(string *fileName)
{
	if (getenv("TEMP") != NULL)
		*fileName = getenv("TEMP") + getFileName(fileName->c_str());
	else if (getenv("TMP") != NULL)
		*fileName = getenv("TMP") + getFileName(fileName->c_str());
	else
		*fileName = SEPARATOR_HEADER + getFileName(fileName->c_str());
}

void cambiarSiEstaEnElRaiz(string *fileName)
{
	if (isInTheRootDirectory(fileName->c_str(), SEPARATOR))
		includePathEnvironment(fileName);
}

string macroExpansion::expand()
{
	newFileName = trimSpaces(tmpnam(NULL));
	cambiarSiEstaEnElRaiz(&newFileName);

	FILE	*fileIn, *fileOut;
	char	line[TAM_BUFFER], oldLine[TAM_BUFFER], auxBuffer[TAM_BUFFER];
	register long	posiRead, posiAux, maxPosiRead = 0;
	register int	huboMacro = 0;
	string   file;

	file = trimSpaces(fileName);
	fileIn = fopen( file.c_str(), "r" );
	MASSERTMSG( fileIn != NULL, "Can't open the file " + fileName );

	fileOut = fopen( newFileName.c_str(), "w+" );
	MASSERTMSG( fileOut != NULL, "Can't open temporary file." );

	while (!feof(fileIn))
	{
		fgets( line, TAM_BUFFER, fileIn );
		maxPosiRead = strlen(line);
		MASSERTMSG( maxPosiRead < TAM_BUFFER - 1, "The file '" + fileName + "' has a line too long");

		if (!feof(fileIn))
		{
			strcpy(auxBuffer, "");		// Clear the buffer
			posiRead = posiAux = 0;
			huboMacro = 0;

			if (line[0] == COMENTARIO_HEADER)
				;
			else if (isInclude(line, posiRead))
				addIncludeFile( getData(line, MACRO_INCLUDE) );
			else
			{
				while (posiRead < maxPosiRead)
				{
					if (line[posiRead] == COMENTARIO_HEADER)
					{
						posiRead = maxPosiRead;
						auxBuffer[posiAux++] = '\n';
					}
					else if (line[posiRead] == MACRO_HEADER)
					{
						if (isMacro(line, posiRead))
						{
							// First: Makes a flush of the auxBuffer
							fwrite(auxBuffer, 1, posiAux, fileOut);
							posiAux = 0;
	
							// Now: Adds the contents of the macro
							putMacroExpansion( fileOut, upcase(getData(line, posiRead, MACRO_MACRO)) );
							posiRead = advanceMacro(line, posiRead);
							huboMacro = 1;
						}
						else
							// Add the char to auxBuffer
							auxBuffer[posiAux++] = line[posiRead];
					}
					else
						// Add the char to auxBuffer
						auxBuffer[posiAux++] = line[posiRead];
	
					posiRead++;
				}

				// Flush the buffer
				if (posiAux != 1 || !huboMacro)
					fwrite(auxBuffer, 1, posiAux, fileOut);
			}
			strcpy(oldLine, line);
		}
	}

	if (strcmp(oldLine, line) != 0)
	{
		strcpy(auxBuffer, "");		// Clear the buffer
		posiRead = posiAux = 0;
		huboMacro = 0;

		if (line[0] == COMENTARIO_HEADER)
			;
		else if (isInclude(line, posiRead))
			addIncludeFile( getData(line, MACRO_INCLUDE) );
		else
		{
			while (posiRead != maxPosiRead)
			{
				if (line[posiRead] == COMENTARIO_HEADER)
				{
					posiRead = maxPosiRead;
					auxBuffer[posiAux++] = '\n';
				}
				else if (line[posiRead] == MACRO_HEADER)
				{
					if (isMacro(line, posiRead))
					{
						// First: Makes a flush of the auxBuffer
						fwrite(auxBuffer, 1, posiAux, fileOut);
						posiAux = 0;

						// Now: Adds the contents of the macro
						putMacroExpansion( fileOut, upcase(getData(line, posiRead, MACRO_MACRO)) );
						posiRead = advanceMacro(line, posiRead);
						huboMacro = 1;
					}
					else
						// Add the char to auxBuffer
						auxBuffer[posiAux++] = line[posiRead];
				}
				else
					// Add the char to auxBuffer
					auxBuffer[posiAux++] = line[posiRead];

				posiRead++;
			}
			// Flush the buffer
			if (posiAux != 1 || !huboMacro)
				fwrite(auxBuffer, 1, posiAux, fileOut);
		}
		strcpy(oldLine, line);
	}

	fclose(fileIn);
	fclose(fileOut);

	return newFileName;
}

char *advanceToChar( char *line, char c )
{
	while( *line != 0 && *line != c)
		line++;

	return line;
}

int macroExpansion::isInclude( char *line )
{
	return isStringSimilar( line, MACRO_INCLUDE );
}

int macroExpansion::isInclude( char *line, long posLine )
{
	return isStringSimilar( line, posLine, MACRO_INCLUDE );
}

int macroExpansion::isMacro( char *line, long posLine )
{
	return isStringSimilar( line, posLine, MACRO_MACRO );
}

int macroExpansion::isBeginOfMacro( char *line )
{
	return isStringSimilar( line, MACRO_BEGINMACRO );
}

int macroExpansion::isEndOfMacro( char *line )
{
	return isStringSimilar( line, MACRO_ENDMACRO );
}

string macroExpansion::getData( char *line, long posLine, string macroCmd)
{
	line = advanceToChar( line + posLine, '(' );

	MASSERTMSG( *line != 0, "Can't find the '(' in the " + macroCmd + " statement.");

	string	sAux("");
	line++;

	while( *line != 0 && *line != ')')
	{
		sAux += *line;
		line++;
	}

	MASSERTMSG( *line != 0, "Can't find the ')' in the " + macroCmd + " statement.");

	// Now, sAux has the data required
	return sAux;
}

void macroExpansion::addIncludeFile( string file )
{
	includeFiles.push_back( file );	
}

void macroExpansion::putMacroExpansion( FILE *fileOut, string macroName )
{
	list<string>::iterator	cursor;

	for (cursor = includeFiles.begin(); cursor != includeFiles.end(); cursor++)
		if (putMacroExpansionInFile( fileOut, macroName, *cursor))
			return;	// TODO OK

	MASSERTMSG( 0, "The macro '" + macroName + "' is undefined.");
}

int macroExpansion::putMacroExpansionInFile( FILE *fileOut, string macroName, string macroFile)
{
	FILE	*fileMacro;
	char	macroLine[TAM_BUFFER];
	string  file(trimSpaces(macroFile));

	fileMacro = fopen( file.c_str(), "r");
	MASSERTMSG( fileMacro != NULL, "Can't open the macro file '" + macroFile + "'.");

	while (!feof(fileMacro))
	{
		fgets( macroLine, TAM_BUFFER, fileMacro );

		if (isBeginOfMacro(macroLine) && upcase(getData(macroLine, MACRO_BEGINMACRO)) == macroName)	// This is the macro what i'm looking for
			return copyMacro( fileOut, fileMacro, macroLine, macroName );
	}

	fclose(fileMacro);
	return 0;		// The macro isn't in this file.
}

int macroExpansion::copyMacro( FILE *fileOut, FILE *fileIn, char *macroLine, string macroName )
{
	while (!feof(fileIn))
	{
		fgets( macroLine, TAM_BUFFER, fileIn );

		if (isEndOfMacro( macroLine ))
		{
			fclose(fileIn);
			return 1;
		}

		fputs( macroLine, fileOut);
	}

	MASSERTMSG( 0, "Can't find the '#EndMacro' statement for the macro '" + macroName + "'.");
	return 0;
}
