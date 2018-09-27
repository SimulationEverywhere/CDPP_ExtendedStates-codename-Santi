/*******************************************************************
*
*  DESCRIPTION: Convert Common .VAL files to .MAP files
*
*  AUTHOR:    Daniel Rodriguez
*
*  EMAIL:     mailto://drodrigu@dc.uba.ar
*
*  DATE: 12/06/1999
*
*******************************************************************/

// ** include files **//
#include <strstream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <cstring>
#include "ini.h"     // Class Ini
#include "VTime.h"    // Class VTime
#include "strutil.h" // str2Int y str2Real
#include "cellstate.h"

using namespace std;

// ** main ** //
void showHelp()
{
	cout << "toMap -[?hmci]\n\n";
	cout << "where:\n";
	cout << "\t?\tShow this message\n";
	cout << "\th\tShow this message\n";
	cout << "\tm\tSpecify file containig the model (.ma)\n";
	cout << "\tc\tSpecify the Cell model within the .ma file\n";
	cout << "\ti\tSpecify the input .VAL file\n";
	exit(1);
}

const char *OutputFileName(string iniName)
{
	const char	*pIniName = iniName.c_str();
	string		out("");

	while ( *pIniName != 0  && *pIniName != '.')
		out += *(pIniName++);

	if (out == "")
	{
		cout << "The Model name has errors\n";
		exit(1);
	}

	out += ".map";

	return out.c_str();
}

inline void WriteValue( fstream *outF, Real r)
{
	*outF << r << "\n";	
}

int splitLine( char *line, char *pos, char *value)
{
        char    *posi = pos, *val = value;

        // Primero leo POS
        while ( *line != 0 && *line != '=' )
        {
                if (*line != ' '){
                        *posi = *line;
                        posi++;
                }
                line++;
        }
        if (*line == 0)
                return 0;

        *posi = 0;
        line++;

        // Ahora leo el value
        while ( *line != 0 )
        {
                if (*line != ' '){
                        *val = *line;
                        val++;
                }
                line++;
        }
        *val = 0;

        if (pos[0] == '(' && strlen(value) > 0)
                return 1;

        return 0;
}

//////////////////////////////////////////////////////////////////////////////
// MAIN - ToMap
//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	try
	{
		string iniName(""), inputFile(""), modelName("");

		// parameter parsing
		while( --argc )
			if( *argv[ argc ] == '-' )
				switch( argv[ argc ][ 1 ] )
				{
					case 'm': /* file .ma */
						iniName = argv[ argc ] + 2;
						break;

					case 'c': /* cell model */
						modelName = argv[ argc ] + 2;
						break;

					case 'i':
						inputFile = argv[ argc ] + 2;
						break;

					case '?':
					case 'h':
						showHelp();

					default:
						cout << "Warning... invalid parameter " << argv[ argc ] << "!" << endl ;
						showHelp();
				}
			else
				cout << "Warning... invalid parameter " << argv[ argc ] << "!"	<< endl ;

		// parameter validation
		if( iniName == "" || inputFile == ""  || modelName == "")
		{
			cout << "Usage: " << argv[ 0 ] << " -mfile.ma -cCellModel -iInputFileName" << endl;
			return 1 ;
		}

		Ini iniFile ;

		iniFile.parse( iniName ) ;

		// dimension
		nTupla			nt;
		register unsigned	cols, rows;
		if (iniFile.exists( modelName, "width" ))
		{
			cols = str2Int( iniFile.definition( modelName, "width"  ).front() );
			rows = str2Int( iniFile.definition( modelName, "height" ).front() );

			nt.add(rows, cols);
		}
		else if (iniFile.exists( modelName, "dim" ))
		{
			Ini::IdList	dimension = iniFile.definition( modelName, "dim" );
			CellPosition	cp( iniFile.join(dimension) );

			nt = cp;

			cols = nt.get(DIM_WIDTH);
			rows = nt.get(DIM_HEIGHT);
		}

		if ( nt.contains(0) )
			MASSERT_ERR( "Attemp to use a model where a component of its dimension is 0" );

		CellState state( nt ) ;

		/////////////////////////////////////////////////////////////
		// Sets the initial value
		/////////////////////////////////////////////////////////////
		string initialValue( iniFile.definition( modelName, "initialvalue" ).front() );
		Real initValue(str2Real(initialValue));

		CellPosition counter;
	        counter.fill(nt.dimension(), 0);
	        register bool	overflow = false;

	        while (!overflow)
	        {
        	        state[counter] = initValue;
	                overflow = counter.next( nt );
	        }

		//////////////////////////////////////////////////////////////
		FILE	*outF;

		outF = fopen(OutputFileName(inputFile), "w");
		if (outF == NULL)
		{
			cout << "Can't open output file";
			unlink(OutputFileName(iniName));
			return -1;
		}
		fclose( outF );

		fstream *outputFile;

		outputFile = new fstream( OutputFileName(inputFile), ios::out );

		//////////////////////////////////////////////////////////////
		FILE *fileIn;
		char line[256], pos[200], value[100];

		MASSERTMSG( fileIn = fopen(inputFile.c_str(), "r"), "Can't open the input file\n");

		while (!feof(fileIn))
		{
			fgets(line, 255, fileIn);
			if (line != NULL & splitLine(line, pos, value))
			{
				CellPosition    cpn(pos);
				state[ cpn ] = str2Real(value);
			}
		}
		fclose( fileIn );

		//////////////////////////////////////////////////////////////////////
	        // Now, saves the result to the OutputFile
        	//////////////////////////////////////////////////////////////////////
	        counter.fill(nt.dimension(), 0);
		overflow = false;

	        while (!overflow)
	        {
        	        WriteValue( outputFile, state[counter] );
	                overflow = counter.next( nt );
	        }
	        /////////////////////////////////////////////////////////////
	} catch( MException &e )
	{
		e.print(cerr);
	} catch( ... )
	{
		cerr << "Unknown exception! " << endl ;
	}
}
