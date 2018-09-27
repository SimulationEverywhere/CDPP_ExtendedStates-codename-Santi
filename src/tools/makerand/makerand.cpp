/*******************************************************************
*
*  DESCRIPTION: Makes a Random file to test the N-CD++ tool
*
*  AUTHOR:    Daniel Rodriguez
*
*  EMAIL:     mailto://drodrigu@dc.uba.ar
*
*  DATE: 30/05/1999
*
*******************************************************************/

// ** include files **//
#include <strstream>
#include <fstream>
#include <unistd.h>
#include <ctime>
#include <cstdlib>
#include <cstring>
#include "ini.h"     // Class Ini
#include "VTime.h"    // Class VTime
#include "strutil.h" // str2Int y str2Real
#include "cellstate.h"

using namespace std;

// ** main ** //
static const unsigned long unifc = 1234567891;  // Estos son los valores de la
static const unsigned long unifa = 2564393833;  // sucesion lineal congruencial
static const unsigned long unifm = 2147483648;  // POTENCIA = 11
static unsigned long unifx0=0;                  // Semilla de nuestra suc.lineal

static unsigned	long	cant0 = 0;
static unsigned long	cant1 = 0;


double Uniforme(void)   // Devuelve un numero al azar segun una
{                       // distribucion uniforme, comprendido entre
                        // 0 y 1 (Nunca alcanza 1, o sea 0<=x<1 )
        time_t tiempo;

        if (unifx0 == 0)
                unifx0 = clock()+time(&tiempo);
        else
                unifx0 = (unifa*unifx0+unifc) % unifm;

        return ((double) unifx0) / ((double) unifm);
}

void showHelp()
{
	cout << "makerand -[?hmcir]\n\n";
	cout << "where:\n";
	cout << "\t?\tShow this message\n";
	cout << "\th\tShow this message\n";
	cout << "\tm\tSpecify file containig the model (.ma)\n";
	cout << "\tc\tSpecify the Cell model within the .ma file\n";
	cout << "\ti\tGenerate integer numbers randomly. The format is:\n";
	cout << "\t \t\t-iquantity[+|-]minNumber[+|-]maxNumber\n";
	cout << "\t \tFor example: \t-i100-10+5 \twill sets 100 cells with\n";
	cout << "\t \tinteger values in the interval [-10,5] with uniform distribution\n";
	cout << "\tr\tGenerate real numbers randomly. The format is:\n";
	cout << "\t \t\t-rquantity[+|-]minNumber[+|-]maxNumber\n";
	cout << "\t \tand its behaviour is similar to the -i parameter.\n";

//	cout << "\ts\tSpecify the value set\n";
//	cout << "\t \t s0   = Use the values 0 & 1 (Uniform Distribution)\n";
//	cout << "\t \t s1-n = Use the value 1 for n cells & 0 for the rest\n";
//	cout << "\t \t s2-n = Makes randoms states for the Pinball Model\n";
//	cout << "\t \t s3-n = Randoms states for the Gas Dispersion Model\n";
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

	out += ".val";

	return out.c_str();
}

inline void WriteValue( fstream *outF, CellPosition counter, Real r)
{
	*outF << counter << " = " << r << "\n";	
}

int GenerateRandomValue( int v )
{
	// This function generates an integer random value between 0 and v
	int	genVal = (int) (Uniforme() * v);

	return (genVal == v) ? GenerateRandomValue(v) : genVal;
}

inline Real ObtenerRandomValue(double v1, double v2)
{
	return Real( (int) (v1 + Uniforme() * v2) );
}

inline Real ObtenerRandValue(bool bInteger, double fMin, double fMax)
{
	return bInteger ? Real( (int) (fMin + Uniforme() * (fMax-fMin+1)) )
			: Real( (double) (fMin + Uniforme() * (fMax - fMin)) );
}

void GenerateRandomTupla(nTupla *dim, nTupla *returned)
{
	for (unsigned long i = 0; i < dim->dimension(); i++)
		returned->setElement(i, GenerateRandomValue( dim->get(i) ) );
}

void AddValues(nTupla *dim, Real value, CellState *state, long cantValues)
{
	nTupla	nt( dim->dimension(), 0 );

	for (long i = 0; i < cantValues; i++)
	{	
		GenerateRandomTupla(dim, &nt);
		(*state)[ nt ] = value;
	}
}

long CountValues(CellState *state, nTupla *dim, Real value)
{
	long	finded = 0;

	//////////////////////////////////////////////////////////////
	// loop on all cells
	CellPosition	counter( dim->dimension(), 0);
	register bool	overflow = false;

	while (!overflow)
	{
		if ( (*state)[counter] == value)
			finded++;

		overflow = counter.next( *dim );
	}
	/////////////////////////////////////////////////////////////

	return finded;
}

//////////////////////////////////////////////////////////////////////////
// Generator functions
//////////////////////////////////////////////////////////////////////////
Real s0(void)
{
	if (Uniforme() < 0.5)
	{
		cant1++;
		return Real(1);
	}
	cant0++;
	return Real(0);
}

inline Real s1(void)
{
	return Real( (int) (1 + Uniforme() * 8) );
}

Real generateValue(string mode)
{
	if (mode == "0")
		return s0();
	else if (mode == "1")
		return s1();
}

void GenerateRandomValues(nTupla nt, fstream *outputFile, string mode)
{
	//////////////////////////////////////////////////////////////
	// loop on all cells
	CellPosition	counter( nt.dimension(), 0);
	register bool	overflow = false;

	while (!overflow)
	{
		WriteValue( outputFile, counter, generateValue(mode));
		overflow = counter.next( nt );
	}
	/////////////////////////////////////////////////////////////

	// Show statistics
	cout << "Mode = S0 - Use only the values 0 and 1 (Uniform Distribution)\n";
	cout << "\t#0 = " << cant0 << "\n";
	cout << "\t#1 = " << cant1 << "\n";
}

void GenerateRandValues(Ini *iniFile, string modelName, nTupla dim,
		fstream *outputFile, bool bInteger, long lCantidad,
		double fMin, double fMax)
{
	CellState state( dim );

	//////////////////////////////////////////////////////////////
	// sets the model with the value ? for all cells
	Real valueUndef;

	CellPosition    counter( dim.dimension(), 0);
	register bool   overflow = false;

	while (!overflow)
	{
		state[ counter ] = valueUndef;
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
        // Now sets the random state
	/////////////////////////////////////////////////////////////
	long	generated = 0;
	long	toGenerate = lCantidad;

	MASSERTMSG( toGenerate <= dim.totalElements(), "The numbers of cells to be generated can't be greater than the number of cells of the model\n");
	/////////////////////////////////////////////////////////////////////
	// Generate the values
	/////////////////////////////////////////////////////////////////////
	while (generated < toGenerate)
	{
		for (long www = 0; www < toGenerate - generated; www++)
			AddValues(&dim, ObtenerRandValue(bInteger, fMin, fMax), &state, 1);

		generated = dim.totalElements() - CountValues(&state, &dim, valueUndef);
	}

	//////////////////////////////////////////////////////////////
	// set the default initial value for the rest cells
	string initialValue( iniFile->definition( modelName, "initialvalue").front());

	Real value ( str2Real( initialValue ) );

	counter.fill(dim.dimension(), 0);
	overflow = false;

	while (!overflow)
	{
		if (state[counter] == valueUndef)
			state[ counter ] = value;

		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////
	// Now, saves the result to the OutputFile
	//////////////////////////////////////////////////////////////////////
	counter.fill(dim.dimension(), 0);
	overflow = false;

	while (!overflow)
	{
		WriteValue( outputFile, counter, state[counter] );
		overflow = counter.next( dim );
	}
}

void GenerateNValues(Ini *iniFile, string modelName, nTupla dim, fstream *outputFile, string mode)
{
	CellState state( dim );

	//////////////////////////////////////////////////////////////
	// default initial value
	string initialValue( iniFile->definition( modelName, "initialvalue").front());

	Real value ( str2Real( initialValue ) );

	CellPosition    counter( dim.dimension(), 0);
	register bool   overflow = false;

	while (!overflow)
	{
		state[ counter ] = value;
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
        // Now sets the initial state
	/////////////////////////////////////////////////////////////
	long	generated = 0;
	long	toGenerate;

	// Get the value passed throught the mode
	char	sValue[100];
	MASSERTMSG( strlen(mode.c_str()) >= 3, "The Mode must be s1-n, with n an integer number\n");
	strcpy(sValue, mode.c_str() + 2);

	toGenerate = atol(sValue);
	MASSERTMSG( toGenerate <= dim.totalElements(), "The numbers of cells to be generated can't be greater than the number of cells of the model\n");

	/////////////////////////////////////////////////////////////////////
	// Generate the values
	/////////////////////////////////////////////////////////////////////
	while (generated < toGenerate)
	{
		AddValues(&dim, Real(1), &state, toGenerate - generated);
		generated = CountValues(&state, &dim, Real(1));
	}

	//////////////////////////////////////////////////////////////////////
	// Now, saves the result to the OutputFile
	//////////////////////////////////////////////////////////////////////
	counter.fill(dim.dimension(), 0);
	overflow = false;

	while (!overflow)
	{
		WriteValue( outputFile, counter, state[counter] );
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	// Show statistics
	cout << "Mode = " << mode << " - Use the value 1 for n cells & 0 for the rest\n";
	cout << "#0    = " << CountValues(&state, &dim, Real(0)) << "\n";
	cout << "#1    = " << CountValues(&state, &dim, Real(1)) << "\n";
	cout << "-----------------------\n";
	cout << "Total = " << dim.totalElements() << "\n";
}

void GenerateNValuesPinball(Ini *iniFile, string modelName, nTupla dim, fstream *outputFile, string mode)
{
	CellState state( dim );

	//////////////////////////////////////////////////////////////
	// default initial value
	string initialValue( iniFile->definition( modelName, "initialvalue").front());

	Real value ( str2Real( initialValue ) );

	CellPosition    counter( dim.dimension(), 0);
	register bool   overflow = false;

	while (!overflow)
	{
		state[ counter ] = value;
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
        // Now sets the initial state
	/////////////////////////////////////////////////////////////
	long	generated = 0;
	long	toGenerate;

	// Get the value passed throught the mode
	char	sValue[100];
	MASSERTMSG( strlen(mode.c_str()) >= 3, "The Mode must be s2-n, with n an integer number\n");
	strcpy(sValue, mode.c_str() + 2);

	toGenerate = atol(sValue);
	MASSERTMSG( toGenerate <= dim.totalElements(), "The numbers of cells to be generated can't be greater than the number of cells of the model\n");

	/////////////////////////////////////////////////////////////////////
	// Generate the values
	/////////////////////////////////////////////////////////////////////
	while (generated < toGenerate)
	{
		AddValues(&dim, Real(9), &state, toGenerate - generated);
		generated = CountValues(&state, &dim, Real(9));
	}

	//////////////////////////////////////////////////////////////////////
	// Now, sets the ball in a random place and with random direction
	//////////////////////////////////////////////////////////////////////
	AddValues(&dim, generateValue("1"), &state, 1);

	//////////////////////////////////////////////////////////////////////
	// Now, saves the result to the OutputFile
	//////////////////////////////////////////////////////////////////////
	counter.fill(dim.dimension(), 0);
	overflow = false;

	while (!overflow)
	{
		WriteValue( outputFile, counter, state[counter] );
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	// Show statistics
	cout << "Mode = " << mode << " - Use the value 9 for n cells\n";
	cout << "#9    = " << CountValues(&state, &dim, Real(9)) << "\n";
	cout << "#0    = " << CountValues(&state, &dim, Real(0)) << "\n";
	cout << "Ball  = 1\n";
	cout << "-----------------------\n";
	cout << "Total = " << dim.totalElements() << "\n";
}

void GenerateNValuesGasDispersion(Ini *iniFile, string modelName, nTupla dim, fstream *outputFile, string mode)
{
	CellState state( dim );

	//////////////////////////////////////////////////////////////
	// default initial value
	string initialValue( iniFile->definition( modelName, "initialvalue").front());

	Real initValue ( str2Real( initialValue ) );

	CellPosition    counter( dim.dimension(), 0);
	register bool   overflow = false;

	while (!overflow)
	{
		state[ counter ] = initValue;
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
        // Now sets the initial state
	/////////////////////////////////////////////////////////////
	long	generated = 0;
	long	toGenerate;

	// Get the value passed throught the mode
	char	sValue[100];
	MASSERTMSG( strlen(mode.c_str()) >= 3, "The Mode must be s3-n, with n an integer number\n");
	strcpy(sValue, mode.c_str() + 2);

	toGenerate = atol(sValue);
	MASSERTMSG( toGenerate <= dim.totalElements(), "The numbers of cells to be generated can't be greater than the number of cells of the model\n");

	/////////////////////////////////////////////////////////////////////
	// Generate the values
	/////////////////////////////////////////////////////////////////////
	while (generated < toGenerate)
	{
		AddValues(&dim, ObtenerRandomValue(1,15), &state, 1);
		generated = dim.totalElements() - CountValues(&state, &dim, initValue);
	}

	//////////////////////////////////////////////////////////////////////
	// Now, saves the result to the OutputFile
	//////////////////////////////////////////////////////////////////////
	counter.fill(dim.dimension(), 0);
	overflow = false;

	while (!overflow)
	{
		WriteValue( outputFile, counter, state[counter] );
		overflow = counter.next( dim );
	}
	/////////////////////////////////////////////////////////////

	// Show statistics
	cout << "Mode = " << mode << " - Generates values for the Gas Dispersion Model\n";
	cout << "#" << initValue.value() << "               = " << CountValues(&state, &dim, initValue) << "\n";
	cout << "#Particles (<>0) = " << dim.totalElements() - CountValues(&state, &dim, initValue) << "\n";
	cout << "-----------------------\n";
	cout << "Total = " << dim.totalElements() << "\n";
}

int getSigno(string format, char *str)
{
	if (*str == '+')
		return 1;
	else if (*str == '-')
		return -1;

	cout << "Error parsing the parameter " << format << "\n";
	exit(-1);
	return 0;	
}

double getNumber(string format, char **str)
{
	int	iEstado = 0;
	string	sNumber = "";
	bool	bFin = false;

	while (!bFin)
	{
		if (iEstado == 0)
		{
			if (isdigit(**str))
			{
				sNumber += **str;
				(*str)++;
				iEstado = 1;
			}
			else if (**str == '.')
			{
				sNumber += **str;
				(*str)++;
				iEstado = 3;
			}
			else
			{
				cout << "Error parsing the string " << format << "\n";
				exit(-1);
			}
		}
		else if (iEstado == 1)
		{
			if (isdigit(**str))
			{
				sNumber += **str;
				(*str)++;
			}
			else if (**str == '.')
			{
				sNumber += **str;
				(*str)++;
				iEstado = 3;
			}
			else if (**str == '+' || **str == '-' || **str == 0)
				iEstado = 2;
			else
			{
				cout << "Error parsing the string " << format << "\n";
				exit(-1);
			}
		}
		else if (iEstado == 2)
			bFin = true;
		else if (iEstado == 3)
		{
			if (isdigit(**str))
			{
				sNumber += **str;
				(*str)++;
				iEstado = 4;
			}
			else
			{
				cout << "Error parsing the string " << format << "\n";
				exit(-1);
			}
		}
		else if (iEstado == 4)
		{
			if (isdigit(**str))
			{
				sNumber += **str;
				(*str)++;
			}
			else if (**str == '+' || **str == '-' || **str == 0)
				iEstado = 2;
			else
			{
				cout << "Error parsing the string " << format << "\n";
				exit(-1);
			}
		}
	}

	return str2float( sNumber );
}

void parserFormat(string format, bool *bInteger, long *lCantidad, double *fMin, double *fMax)
{
	char	*str = (char *) format.c_str();
	int	iSigno;

	*bInteger = (*str == 'I' || *str == 'i')?true:false;
	str++;

	*lCantidad = (long) getNumber(format, &str);

	iSigno = getSigno(format, str);
	str++;

	*fMin = iSigno * getNumber(format, &str);

	iSigno = getSigno(format, str);
	str++;

	*fMax = iSigno * getNumber(format, &str);
}

//////////////////////////////////////////////////////////////////////////////
// MAIN - MAKERAND
//////////////////////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	try
	{
		string iniName(""), mode(""), modelName(""), format("");

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

					case 's':
						mode = argv[ argc ] + 2;
						break;

					case 'i':
					case 'r':
						format = argv[ argc ] + 1;
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
		if( iniName == "" || modelName == "" || (mode == "" && format == ""))
		{
			cout << "Usage: " << argv[ 0 ] << " -mfile.ma -cCellModel -sOutputFormat" << endl;
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

		FILE	*outF;

		outF = fopen(OutputFileName(iniName), "w");
		if (outF == NULL)
		{
			cout << "Can't open output file";
			unlink(OutputFileName(iniName));
			return -1;
		}
		fclose( outF );

		fstream *outputFile;

		outputFile = new fstream( OutputFileName(iniName), ios::out );

		if (format != "")
		{
			bool bInteger;
			long lCantidad;
			double fMin, fMax;

			parserFormat(format, &bInteger, &lCantidad, &fMin, &fMax);
			GenerateRandValues(&iniFile, modelName, nt, outputFile, bInteger, lCantidad, fMin, fMax);
		}

		//////////////////////////////////////////////////////////////
		// Applies a method depending the selected mode
		//////////////////////////////////////////////////////////////
		if (format == "")
			if (mode == "0")
				GenerateRandomValues(nt, outputFile, mode);
			else if (*(mode.c_str()) == '1')
				GenerateNValues(&iniFile, modelName, nt, outputFile, mode);
			else if (*(mode.c_str()) == '2')
				GenerateNValuesPinball(&iniFile, modelName, nt, outputFile, mode);
			else if (*(mode.c_str()) == '3')
				GenerateNValuesGasDispersion(&iniFile, modelName, nt, outputFile, mode);
		//////////////////////////////////////////////////////////////

		cout << "File OK\n";
	} catch( MException &e )
	{
		e.print( cerr ) ;
	} catch( ... )
	{
		cerr << "Unknown exception! " << endl ;
	}
}
