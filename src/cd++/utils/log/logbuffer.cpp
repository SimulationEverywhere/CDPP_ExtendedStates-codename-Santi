/*******************************************************************
*
*  DESCRIPTION: Buffers and sorts the log output from different LPs and produces
*		an ordered list of events.
*
*  AUTHOR:    Alejandro Troccoli
*
*  EMAIL: mailto://atroccol@dc.uba.ar
*
*  DATE: 19/04/2001
*
*******************************************************************/

#include <queue>
#include <string>
#include <vector>
#include <fstream>
#include <cstring>
#include "VTime.h"
#include "logparser.h"

using namespace std;

/******************************************************************
* class LogLine
******************************************************************/
class LogLine 
{

public:
	friend class PrioritizeLogLine;

	LogLine( VTime t, char* l ) {
		line = new char[128];
		strcpy(line,l); 
		time = t;
	}

	char *getLine() const 
	{	return line; }

	~LogLine() 
	{
		delete line;
	}
private:
	char *line;
	VTime time;
};


/******************************************************************
* class PrioritizeLogLine
******************************************************************/

class PrioritizeLogLine
{
public :
	int operator()( const LogLine *x, const LogLine *y )
	{
		return y->time <  x->time;
	}
};

/******************************************************************
* Main
******************************************************************/

int main( int argc, char* argv[] ) 
{
	char buffer[2048];

	unsigned int bufferSize = 200;
	string strBufferSize;
	// parameter parsing

	while( --argc )
		if( *argv[ argc ] == '-' )
			switch( argv[ argc ][ 1 ] )
		{
			case 'b': /* file .ma */
				strBufferSize = argv[ argc ] + 2;
				bufferSize = str2Int( strBufferSize );
				break;

			default:
				cout << "Warning... invalid parameter " << argv[ argc ] << "!" << endl ;
		}
		else
			cout << "Warning... invalid parameter " << argv[ argc ] << "!"	<< endl ;

	priority_queue<LogLine*, vector< LogLine* >, PrioritizeLogLine> q;
	istream* in = &cin;
	//istream* in = new fstream ( "calor2.log", ios::in) ;

	while( !in->eof() && in->good() )
	{
		int LP;
		VTime time;
		bool valid;

		in->getline( buffer, 2048 );
		valid = isMessageLine( buffer, LP, time);

		//cout << " Analizando: "  << buffer;

		if ( valid )
		{
			//cout << " valida! Time:" << time << endl;
			q.push( new LogLine( time, buffer) );

			if ( q.size() == bufferSize )
			{
				cout << q.top()->getLine() << endl;
				delete q.top();
				q.pop();

			}

		} 

	} //while

	while (  !q.empty() )
	{
		cout << q.top()->getLine() << endl;
		delete q.top();
		q.pop();
	}//while

}
