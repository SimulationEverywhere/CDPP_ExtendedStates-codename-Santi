/*******************************************************************
*
*  DESCRIPTION: Analizes the log file produced by PCD++ to determine the level
*		  of parallelism in the model.
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: 	mailto://atroccol@dc.uba.ar
*
*  DATE: 	21/04/2001
*
*******************************************************************/

#include <fstream>
#include <map>
#include "pini.h"
#include "logparser.h"

using namespace std;

void showHelp()
{
	cout << "PARLOG: An utility to determine the level of parallelism" << endl;
	cout << "usage: parlog -[?hmP]\n\n";
	cout << "where:\n";
	cout << "\t?\tShow this message\n";
	cout << "\th\tShow this message\n";
	cout << "\tP\tSpecifies the model partition file (required)\n";
	exit(1);
}

int
	main ( int argc, char *argv[] )
{
	typedef map< int, int, less < int > > LPStats;

	PIni ini;
	LPStats stats;
	string parName("");

	// parameter parsing

	while( --argc )
		if( *argv[ argc ] == '-' )
			switch( argv[ argc ][ 1 ] )
		{
			case 'P': /* file .ma */
				parName = lowerCase(argv[ argc ] + 2);
				break;

			default:
				cerr << "Warning... invalid parameter " << argv[ argc ] << "!" << endl ;
				showHelp();
		}
		else
			cerr << "Warning... invalid parameter " << argv[ argc ] << "!"	<< endl ;


	if ( parName == "" ) 
	{
		cerr << "Warning... missing parameter -P" << endl;
		showHelp();
	}	

	istream* in = &cin;	
	VTime currentTime( VTime::Zero ), logTime( VTime::Zero ) ;
	string msgType;
	int LP;
	char buffer[2048];

	ini.parse(parName);

	//Add all machines to the stats list and print the header
	cout << "Time/LP\t";

	for( PIni::MachineList::const_iterator cursor = ini.machines().begin(); 
	cursor != ini.machines().end(); cursor++ )
	{
		cout << cursor->first << "\t";
		stats[ cursor->first ] = 0;
	}

	cout << endl;

	while ( in->good() && !in->eof() )
	{
		in->getline( buffer, 2048);
		if ( isMessageLine(buffer, LP, logTime, msgType))
		{
			if ( currentTime != logTime ) 
			{
				cout << currentTime << "\t";

				for( unsigned int lp = 0; lp < stats.size(); lp++)
				{
					cout << stats[ lp ] << "\t";
					stats[ lp ] = 0;
				}	

				cout << endl;
				currentTime = logTime;
			}

			if ( msgType == "*" )
			{
				stats[LP]++;
			}
		}//if
	}//while

	//Print the last line
	cout << currentTime << "\t";

	for( unsigned int lp = 0; lp < stats.size(); lp++)	
	{
		cout << stats[ lp ] << "\t";
		stats[ lp ] = 0;
	}	

	cout << endl;

	return 0;
}//main


