/*******************************************************************
*
*  DESCRIPTION: class StandAloneLoader
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel A. Rodriguez.
*  Version 3: Gabriel Wainer
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*         mailto://gabrielw@dc.uba.ar
*	  mailto://atroccol@dc.uba.ar
*
*  DATE: 27/6/1998
*  DATE: 29/4/1999
*  DATE: 6/2/2000
*  DATE: 29/08/2000
*
*******************************************************************/

/** include files **/
#include <fstream>
#include <strstream>     // class strstream
#include <ctime>
#include <math.h>
#include "stdaload.h"      // base class
#include "realprec.h"
#include "impresion.h"
#include "log.h"
#include <string.h>

#ifdef MPI
#include "mpi.h"			//MPI_Routines
#endif

#include "LogicalProcess.h"	// Warped logical process

extern "C"
{
#include "randlib.h"
}

using namespace std;

#define	CTE_STR_W	"The format for the -w parameter is: -wXX-YY where XX and YY are integer numbers."

#define CHECK_ARG(opt, arg) {\
	if(arg == nullptr)\
	{\
		cout << "Option -" << opt << " requires argument!" << endl;\
		exit(1);\
	}\
}

int getWidth( const char *str )
{
	char	aux[100];
	int	i = 0;

	while (*str != 0 && *str != '-')
	{
		aux[i++] = *str;
		str++;
	}
	MASSERTMSG(*str == '-', CTE_STR_W );

	aux[i] = 0;
	return atoi(aux);
}

int getPrecision( const char *str )
{
	char	aux[100];
	int	i = 0;

	while (*str != 0 && *str != '-')
		str++;

	MASSERTMSG(*str == '-', CTE_STR_W );
	str++;

	while (*str != 0)
	{
		aux[i++] = *str;
		str++;
	}
	aux[i] = 0;
	return atoi(aux);
}


/*******************************************************************
* Function Name: StandAloneLoader
* Description: Default Constructor
********************************************************************/
StandAloneLoader::StandAloneLoader(int argc, char** argv)
:SimLoader( argc, argv)
, iniName( "model.ma" )
	, evName( "" )
	, evalDebugName( "/dev/null" )
	, flatDebugName( "/dev/null" )
	, parserFileName( "stdout" )
	, partitionFileName( "" )
{
}

/*******************************************************************
* Function Name: openCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &StandAloneLoader::openComm(){

#ifdef MPI	

	//Init MPI.If not start up machine, this will update
	//the command line arguments
	physicalCommInit(&argc , &argv);

	machineID = physicalCommGetId();

	//cout << "MPI ID: " << machineID << endl;
#else

	machineID = 0;

#endif

	readCommandLine();

	return *this;
}

/*******************************************************************
* Function Name: closeCommChanel
* Opens the communications channel. Gets the machine ID
********************************************************************/
SimLoader &StandAloneLoader::closeComm(){
#ifdef MPI
	MPI_Finalize();
#endif
	return *this;
}

/*******************************************************************
* Function Name: readCommandLine
* Description: reads the arguments from the command line.
********************************************************************/
void StandAloneLoader::readCommandLine() {

	stopTime( VTime::Inf ) ;
	printParserInfo(false);
	preprocessor = 1;
	DebugCellRules().Active( false );
	UseQuantum().Active( false );

	Impresion::Default.Width( 12 );
	Impresion::Default.Precision( 5 );

	while( --argc )
	{
		if( argv[ argc ][ 0 ] == '-' || argv[ argc-1 ][ 0 ] == '-' )
		{
			ssize_t arg_offset, opt_idx, arg_idx;
			char *arg = nullptr;

			// 1. Current string contains option without argument.
			if(argv[ argc ][ 0 ] == '-' && strlen(argv[ argc ]) == 2)
				opt_idx = argc, arg_idx = -1;
			// 2. Current string contains option with argument.
			else if(argv[ argc ][ 0 ] == '-' && strlen(argv[ argc ]) > 2)
				opt_idx = argc, arg_idx = argc, arg_offset = 2;
			// 3. Current string is argument of the option in next string.
			else if(argv[argc-1][0] == '-' && strlen(argv[ argc-1 ]) == 2)
				opt_idx = argc-1, arg_idx = argc, arg_offset = 0, argc--;
			else
			{
				cout << "Invalid parameter " << argv[ argc ] << "!" << endl ;
				exit(1);
			}

			if(arg_idx >= 0)
				arg = argv[ arg_idx ] + arg_offset;

			switch( argv[ opt_idx ][ 1 ] )
			{
				case 'm': // file de modelos a cargar
					iniName = arg ;
					break;

				case 't': // hora de finalizacion de la simulacion
					CHECK_ARG('t', arg);
					stopTime( VTime( arg ) ) ;
					break;

				case 'l': // log name
					CHECK_ARG('l', arg);
					Log::Default.createLog( true );
					Log::Default.filename( arg );
					break;

				case 'L': //log modifiers
					CHECK_ARG('L', arg);
					Log::Default.logType( arg );
					break;

				case 'e': // event file
					CHECK_ARG('e', arg);
					evName = arg;
					break;

				case 'o': // output stream
					CHECK_ARG('o', arg);
					output = arg;
					break;

				case 'v': // evaluate debug mode
					CHECK_ARG('v', arg);
					evalDebugName = arg;
					break;

				case 'f': // evaluate debug mode
					CHECK_ARG('f', arg);
					flatDebugName = arg;
					break;

				case 'h': // help
					cout << argv[ 0 ] << " [-ehlmotdpvbfrsqw]" << endl
						<< "\t" << "e: events file (default: none)" << endl
						<< "\t" << "h: show this help" << endl 
						<< "\t" << "l: logs all messages to a log file (default: /dev/null)" << endl
						<< "\t" << "L[I*@XYDS]: log modifiers (logs only the specified messages) " << endl
						<< "\t" << "m: model file (default : model.ma)" << endl
						<< "\t" << "o: output (default: /dev/null)" << endl
						<< "\t" << "t: stop time (default: Infinity)" << endl
						<< "\t" << "d: set tolerance used to compare real numbers" << endl
						<< "\t" << "p: print extra info when the parsing occurs (only for cells models)" << endl
						<< "\t" << "D: partition details file (default: /dev/null)" << endl
						<< "\t" << "P: parallel partition file (will run parallel simulation)" << endl
						<< "\t" << "v: evaluate debug mode (only for cells models)" << endl
						<< "\t" << "b: bypass the preprocessor (macros are ignored)" << endl
						<< "\t" << "f: flat debug mode (only for flat cells models)" << endl
						<< "\t" << "r: debug cell rules mode (only for cells models)" << endl
						<< "\t" << "s: show the virtual time when the simulation ends (on stderr)" << endl
						<< "\t" << "q: use quantum to compute cell values" << endl
						<< "\t" << "y: use dynamic quantum (strategy 1) to compute cells values" << endl
						<< "\t" << "Y: use dynamic quantum (strategy 2) to compute cells values" << endl
						<< "\t" << "w: sets the width and precision (with form xx-yy) to show numbers" << endl;

					exit(0);
					break;

				case 'p': // Print extra parsing info
					CHECK_ARG('p', arg);
					printParserInfo(true);
					parserFileName = arg;
					ParserDebug().Active( true );
					break;

				case 'P': //Run parallel simulation using filename with partition
					CHECK_ARG('P', arg);
					partitionFileName = arg ;
					break;

				case 'D': //Partition debug file name
					CHECK_ARG('D', arg);
					partitionDebug = arg;
					break;

				case 'r': // Debug Cells rules
					DebugCellRules().Active( true );
					break;

				case 's':  // Show virtual time when simulation ends
					ShowVirtualTimeWhenFinish().Active( true );
					break;

				case 'b': // bypass the preprocessor
					preprocessor = 0;
					break;

				case 'd': // Set the value of TOL
					CHECK_ARG('d', arg);
					RealPrecision::Tol.precision( atof( arg ) );
					break;

				case 'w': // set the width & precision to show real numbers
					CHECK_ARG('w', arg);
					Impresion::Default.Width( getWidth( arg ));
					Impresion::Default.Precision( getPrecision( arg ));
					break;

				case 'y': // If we use dynamic quantum, an initial value is defined
					CHECK_ARG('y', arg);
					UseDynQuantum().Ratio( atof( arg ) );
					UseDynQuantum().Active( true );
					UseDynQuantum().Strat( true );
					break;

				case 'Y': // If we use dynamic quantum, an initial value is defined
					CHECK_ARG('Y', arg);
					UseDynQuantum().Ratio( atof( arg ) );
					UseDynQuantum().Active( true );
					UseDynQuantum().Strat( false );
					break;

				case 'q': // Set the value of the Quantum
					CHECK_ARG('q', arg);
					UseQuantum().Value( atof( arg ) );
					UseQuantum().Active( true );
					break;

				default:
					cout << "Invalid parameter " << argv[ opt_idx ] << "!" << endl ;
					exit(0);
			}
		} else
		{
			cout << "Invalid parameter " << argv[ argc ] << "!" << endl ;
			exit(0);
		}
	}
}

/*******************************************************************
* Function Name: ~StandAloneLoader
********************************************************************/
StandAloneLoader::~StandAloneLoader()
{
}

/** private functions **/

/*******************************************************************
* Function Name: loadData
********************************************************************/
SimLoader &StandAloneLoader::loadData()
{

	ostream *temp;

	//Only print the start screen once
	if ( getMachineID() == 0 )
		temp = &cout;
	else
		temp = new strstream();

	ostream& out = *temp;

	out << "PCD++: A Tool to Implement n-Dimensional Cell-DEVS models\n";
	out << "Version 3.0 - March 2003\n";
	out << "Troccoli A., Rodriguez D., Wainer G., Barylko A., Beyoglonian J., Lopez A.\n";
	out << "-----------------------------------------------------------------------------\n";
	out << "PCD++ Extended States: An extended and improved version of CD++ for Cell-DEVS\n";
	out << "Version 4.1.2 - December 2018\n";
	out << "Santi L., Castro, R., Pimás, J.\n";
	out << "-----------------------------------------------------------------------------\n";
	out << "Discrete Event Simulation Lab\n";
	out << "Departamento de Computación\n";
	out << "Facultad de Ciencias Exactas y Naturales\n";
	out << "Universidad de Buenos Aires, Argentina\n";
	out << "-----------------------------------------------------------------------------\n";
#ifdef MPI
	out << "Compiled for parallel simulation\n" << endl;
#else
	out << "Compiled for standalone simulation\n" << endl;
#endif
	out << "\n";

	out << "Loading models from " << iniName << endl ;
	out << "Loading events from " << evName << endl ;
	out << "Running parallel simulation. Reading models partition from "
		<< partitionFileName
		<< endl;
	out << "Model partition details output to: " << partitionDebug << "*" << endl;
	out << "Message log: " << Log::Default.filename() << endl ;
	out << "Output to: " << output << endl ;
	out << "Tolerance set to: " <<  RealPrecision::Tol.precision() << endl;

	MASSERTMSG( RealPrecision::Tol.precision() >= 0, "The precision can't be negative.");

	out << "Configuration to show real numbers: Width = " << Impresion::Default.Width() << " - Precision = " <<  Impresion::Default.Precision() << "\n";

	out << "Quantum: ";
	if (UseQuantum().Active())
		out <<  UseQuantum().Value() << "\n";
	else
		out << "Not used\n";

	out << "Evaluate Debug Mode = ";
	if (evalDebugName == "/dev/null")
	{
		out << "OFF\n";
		EvalDebug().Active( false );
	}
	else
	{
		out << "ON\tOutput of evaluated data = " << evalDebugName << endl;
		EvalDebug().Active( true );
	}

	out << "Flat Cell Debug Mode = ";
	if (flatDebugName == "/dev/null")
	{
		out << "OFF\n";
		FlatDebug().Active( false );
	}
	else
	{
		out << "ON\tOutput of flat cells debug data = " << flatDebugName << endl;
		FlatDebug().Active( true );
	}

	out << "Debug Cell Rules Mode = " <<  (DebugCellRules().Active() ? "ON":"OFF") << "\n";

	if (!preprocessor)
	{
		out << "Bypassing preprocessor.\n";
		models = new fstream( iniName.c_str(), ios::in );
	}
	else
	{
		MacroExpansion(iniName);
		models = new fstream( MacroExpansion().expand().c_str(), ios::in )  ;
		out << "Temporary File created by Preprocessor = " << MacroExpansion().tempFileName() << "\n";
	}

	out << "Printing parser information = ";
	if (printParserInfo())
	{
		out << "ON" << "\tUsing file = " << parserFileName << "\n";
	}
	else
	{
		out << "OFF\n";
	}

	out << "\nStop at time: ";
	if (stopTime() == VTime::Inf)
		out << "Infinity.";
	else
		out << stopTime().asString();
	out << endl ;

	if( evName == "" )
		events = new strstream() ;
	else
		events = new fstream( evName.c_str(), ios::in );

	if( partitionFileName == "" )
		partition = new strstream();
	else
		partition = new fstream( partitionFileName.c_str(), ios::in );

	if( evalDebugName == "-" || evalDebugName == "" )
		EvalDebug().Stream( &cout );
	else
		EvalDebug().Stream( new fstream( evalDebugName.c_str(), ios::out ) );

	if( flatDebugName == "-" || flatDebugName == "" )
		FlatDebug().Stream( &cout );
	else
		FlatDebug().Stream( new fstream( flatDebugName.c_str(), ios::out ) );

	if( parserFileName == "-" || parserFileName == "stdout" )
		ParserDebug().Stream( &cout );
	else
		ParserDebug().Stream( new fstream( parserFileName.c_str(), ios::out ) );

	if ( getMachineID() != 0 )
		delete temp;

	return *this;   
}

/*******************************************************************
* Function Name: writeResults
********************************************************************/
SimLoader &StandAloneLoader::writeResults()
{
	if (getMachineID() == 0) {
		cout << "Simulation ended!" << endl ;
	}

	delete models ;  models = NULL;
	delete events ;  events = NULL; 

	if (preprocessor)
		delete &(MacroExpansion());

	return *this;   

}
