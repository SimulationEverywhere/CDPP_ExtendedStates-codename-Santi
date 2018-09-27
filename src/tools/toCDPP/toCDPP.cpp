/*******************************************************************
*
*  DESCRIPTION: Convert Common .VAL files to .MA model definition files
*						used by CD++
*
*  AUTHOR:    Daniel Rodriguez
*
*  EMAIL:     mailto://drodrigu@dc.uba.ar
*
*  DATE: 26/06/1999
*
*******************************************************************/

// ** include files **//
#include <sstream>
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
	cout << "toCDPP -[?hmcio]\n\n";
	cout << "where:\n";
	cout << "\t?\tShow this message\n";
	cout << "\th\tShow this message\n";
	cout << "\tm\tSpecify the input file containig the model (.ma)\n";
	cout << "\tc\tSpecify the Cell model within the .ma file\n";
	cout << "\ti\tSpecify the input .VAL file\n";
	cout << "\to\tSpecify the output .MA file\n";
	exit(1);
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

void almacenarValoresModelo(FILE *outF, nTupla &nt, CellState &state)
{
	nTupla	csAux;
	Real	auxVal;

	for (long i = 0; i < nt.get(0); i++)
	{
		fprintf( outF, "initialrowvalue : %ld\t\t", i);

		for (long j = 0; j < nt.get(1); j++)
		{
			csAux.setElement(0,i);
			csAux.setElement(1,j);

			auxVal = state[csAux];

			fprintf( outF, "%c", auxVal.IsUndefined() ? '?' : (auxVal.value() == 0 ? '0' : '1') );
	        }
		fprintf( outF, "\n");
	}
}

char *ModelName(char *linea)
{
	static char	aux[300];
	long	i = 0;

	aux[0] = 0;

	while (*linea != '[' && *linea != 0)
		linea++;

	if (*linea == 0)
		return aux;

	linea++;

	while (*linea != ']' && *linea != 0)
	{
		aux[i++] = *linea;
		linea++;
	}

	aux[i] = 0;

	if (*linea == 0)
		aux[0] = 0;

	return aux;
}

bool empiezaUnNuevoModelo(char *linea)
{
	while (*linea != '[' && *linea != 0)
		linea++;

	return (*linea == 0) ? false : true;
}

bool esCargaValor(char *linea)
{
	while (*linea == ' ' || *linea == '\t')
		linea++;

	if (	strncasecmp( linea, "INITIALROW", 10) == 0 ||
		strncasecmp( linea, "INITIALCELLSVALUE", 17) == 0 ||
		strncasecmp( linea, "INITIALMAPVALUE", 15) == 0 )
			return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////
// MAIN - ToCDPP
//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	try
	{
		string iniName(""), inputFile(""), modelName(""), outputFile("");

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

					case 'o':
						outputFile = argv[ argc ] + 2;
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
		if( iniName == "" || inputFile == ""  || modelName == "" || outputFile == "")
		{
			cout << "Usage: " << argv[ 0 ] << " -mfile.ma -cCellModel -iInputFileName -oOutputFileName" << endl;
			return 1 ;
		}

		Ini iniFile;

		iniFile.parse( iniName );

		// dimension
		nTupla			nt;
		register unsigned	cols, rows;
		if (iniFile.exists( modelName, "width" ))
		{
			cols = str2Int( iniFile.definition( modelName, "width"  ).front() );
			rows = str2Int( iniFile.definition( modelName, "height" ).front() );

			nt.add(rows, cols);
		}
		else
			MASSERT_ERR( "The input .MA file don't have the WIDTH clause");

		if ( nt.contains(0) )
			MASSERT_ERR( "Attemp to use a model where a component of its dimension is 0" );

		CellState state( nt );

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

		outF = fopen(outputFile.c_str(), "w");
		if (outF == NULL)
		{
			cout << "Can't open output file";
			unlink(outputFile.c_str());
			return -1;
		}

		fstream *fOutputFile;

		fOutputFile = new fstream( outputFile.c_str(), ios::out );

		//////////////////////////////////////////////////////////////
		FILE *fileRead;
		char line[256], pos[200], value[100];

		MASSERTMSG( fileRead = fopen(inputFile.c_str(), "r"), "Can't open the input file\n");

		while (!feof(fileRead))
		{
			fgets(line, 255, fileRead);
			if (line != NULL & splitLine(line, pos, value))
			{
				CellPosition    cpn(pos);
				MASSERTMSG( *value == '0' || *value == '1' || *value == '?', "Value not supported in CD++");
				state[ cpn ] = str2Real(value);
			}
		}
		fclose( fileRead );

		//////////////////////////////////////////////////////////////////////
	        // Now, saves the result to the outputFile
        	//////////////////////////////////////////////////////////////////////
		FILE *fileIni;
		bool encontreModelo = false;
		bool empiezaNuevoModelo = false;
		bool almaceneValores = false;

		MASSERTMSG( fileIni = fopen(iniName.c_str(), "r"), "Can't open the input .MA file\n");

		// Copio desde el inicio del archivo hasta que encuentro el modelo
		while (!feof(fileIni) && !encontreModelo)
		{
			fgets(line, 255, fileIni);

			if (strcasecmp(ModelName(line), modelName.c_str()) == 0)
				encontreModelo = true;

			if (!feof(fileIni))
				fputs(line, outF);
		}

		// Copio el modelo
		if (encontreModelo)
			while (!feof(fileIni) && !empiezaNuevoModelo)
			{
				fgets(line, 255, fileIni);

				if (empiezaUnNuevoModelo(line))
				{
					if (!feof(fileIni))
						fputs(line, outF);

					empiezaNuevoModelo = true;
				}
				else if (esCargaValor(line))
				{
					if (!almaceneValores)
					{
						almacenarValoresModelo(outF, nt, state);
						almaceneValores = true;
					}
				}
				else if (!feof(fileIni))
					fputs(line, outF);
			}		

		// Copio a partir del modelo hasta el fin del archivo
		while (!feof(fileIni))
		{
			fgets(line, 255, fileIni);

			if (!feof(fileIni))
				fputs(line, outF);
		}

		fclose( outF );
		fclose( fileIni );
	        /////////////////////////////////////////////////////////////
	} catch( MException &e )
	{
		e.print(cerr);
	} catch( ... )
	{
		cerr << "Unknown exception! " << endl ;
	}
}
