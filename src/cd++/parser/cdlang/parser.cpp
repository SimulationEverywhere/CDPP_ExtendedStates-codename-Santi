/*******************************************************************
*
*  DESCRIPTION: main for parsing
*
*  AUTHOR:    Amir Barylko & Jorge Beyoglonian 
*  Version 2: Daniel Rodriguez.
*
*  EMAIL: mailto://amir@dc.uba.ar
*         mailto://jbeyoglo@dc.uba.ar
*         mailto://drodrigu@dc.uba.ar
*
*  DATE: 27/06/1998
*  DATE: 25/04/1999 (v2)
*
*******************************************************************/

// ** include files **//
#include <iostream>
#include "parser.h"        // class Parser
#include "synnode.h"       // class SyntaxNode
#include "tuple_node.h"
#include "gram.h"          // defines AND, OR, etc
#include "strutil.h"       // lowercase
#include "tbool.h"         // TValBool
#include "mathincl.h"	   // Constants M_PI and M_E

using namespace std;

// ** The prefix character for the state variables ** //
#define VAR_PREFIX      '$'
#define PORT_PREFIX	'~'


// ** declarations ** //
int yyparse() ;

inline
bool esCharStrValido(char c)
{
	return isdigit(c) || ( c >=  '<' && c <= 'z');
}

struct IsLetterPredicate: public unary_function< char, char >
{
	bool operator ()( char c ) const
	{return ( c >= 'A' && c <= 'Z') || ( c >= 'a' && c <= 'z') ;}
};

struct IsBlankPredicate: public unary_function< char, char >
{
	bool operator ()( char c ) const
	{ return c == ' ' || c == '\t';}
};

struct IsDigitPredicate: public unary_function< char, char >
{
	bool operator ()( char c ) const
	{ return c >= '0' && c <= '9';}
};

template < class Iterator, class Predicate >
bool advance_while( Iterator begin, Iterator end, Predicate pred )
{
	while( begin != end && pred( *begin ) )
		begin++;
	return begin == end;
}

template < class Iterator >
bool isLetter( Iterator begin, Iterator end )
{
	IsLetterPredicate pred ;
	return advance_while( begin, end, pred ) ;
}

template < class Iterator >
bool isBlank( Iterator begin, Iterator end )
{
	return advance_while( begin, end, IsBlankPredicate() ) ;
} 

template < class Iterator >
bool isDigit( Iterator begin, Iterator end )
{
	return advance_while( begin, end, IsDigitPredicate() ) ;
} 


Parser *SingleParser::instance( NULL ) ;

Parser &SingleParser::Instance()
{
	if( !instance )
		instance = new Parser();

	return *instance ;
}

/*******************************************************************
* Method: parser
* Description: constructor
********************************************************************/
Parser::Parser() 
: cursor( NULL )
, read( true )
, spec( NULL )
, printParserInfo ( false )
{
	dict[ "t" ] = ValuePair( BOOL, new ConstantNode( TValBool::ttrue, BoolType::TheBool ) ) ; 
	dict[ "f" ] = ValuePair( BOOL, new ConstantNode( TValBool::tfalse, BoolType::TheBool ) ) ; 
	dict[ "?" ] = ValuePair( UNDEF, new ConstantNode( TValBool::tundef, RealType::TheReal ) ) ;

	dict[ "pi" ] = ValuePair( CONSTFUNC, new ConstantNode( M_PI, RealType::TheReal ) ) ;
	dict[ "e" ] = ValuePair( CONSTFUNC, new ConstantNode( M_E, RealType::TheReal ) ) ;
	dict[ "inf" ] = ValuePair( CONSTFUNC, new ConstantNode( M_INFINITO, RealType::TheReal ) ) ;
	dict[ "-inf" ] = ValuePair( CONSTFUNC, new ConstantNode( M_MINFINITO, RealType::TheReal ) ) ;
	dict[ "golden" ] = ValuePair( CONSTFUNC, new ConstantNode( M_GOLDEN, RealType::TheReal ) ) ;
	dict[ "catalan" ] = ValuePair( CONSTFUNC, new ConstantNode( M_CATALAN, RealType::TheReal ) ) ;
	dict[ "rydberg" ] = ValuePair( CONSTFUNC, new ConstantNode( M_RYDBERG, RealType::TheReal ) ) ;
	dict[ "grav" ] = ValuePair( CONSTFUNC, new ConstantNode( M_GRAV, RealType::TheReal ) ) ;
	dict[ "bohr_radius" ] = ValuePair( CONSTFUNC, new ConstantNode( M_BOHR_RADIUS, RealType::TheReal ) ) ;
	dict[ "bohr_magneton" ] = ValuePair( CONSTFUNC, new ConstantNode( M_BOHR_MAGNETON, RealType::TheReal ) ) ;
	dict[ "boltzmann" ] = ValuePair( CONSTFUNC, new ConstantNode( M_BOLTZMANN, RealType::TheReal ) ) ;
	dict[ "accel" ] = ValuePair( CONSTFUNC, new ConstantNode( M_ACCEL, RealType::TheReal ) ) ;
	dict[ "light" ] = ValuePair( CONSTFUNC, new ConstantNode( M_LIGHT, RealType::TheReal ) ) ;
	dict[ "electron_charge" ] = ValuePair( CONSTFUNC, new ConstantNode( M_ELECTRON_CHARGE, RealType::TheReal ) ) ;
	dict[ "planck" ] = ValuePair( CONSTFUNC, new ConstantNode( M_PLANCK, RealType::TheReal ) ) ;
	dict[ "avogadro" ] = ValuePair( CONSTFUNC, new ConstantNode( M_AVOGADRO, RealType::TheReal ) ) ;
	dict[ "AMU" ] = ValuePair( CONSTFUNC, new ConstantNode( M_AMU, RealType::TheReal ) ) ;
	dict[ "PEM" ] = ValuePair( CONSTFUNC, new ConstantNode( M_PEM, RealType::TheReal ) ) ;
	dict[ "ideal_gas" ] = ValuePair( CONSTFUNC, new ConstantNode( M_IDEAL_GAS, RealType::TheReal ) ) ;
	dict[ "faraday" ] = ValuePair( CONSTFUNC, new ConstantNode( M_FARADAY, RealType::TheReal ) ) ;
	dict[ "stefan_boltzmann" ] = ValuePair( CONSTFUNC, new ConstantNode( M_STEFAN_BOLTZMANN, RealType::TheReal ) ) ;
	dict[ "electron_mass" ] = ValuePair( CONSTFUNC, new ConstantNode( M_ELECTRON_MASS, RealType::TheReal ) ) ;
	dict[ "proton_mass" ] = ValuePair( CONSTFUNC, new ConstantNode( M_PROTON_MASS, RealType::TheReal ) ) ;
	dict[ "neutron_mass" ] = ValuePair( CONSTFUNC, new ConstantNode( M_NEUTRON_MASS, RealType::TheReal ) ) ;
	dict[ "euler_gamma" ] = ValuePair( CONSTFUNC, new ConstantNode( M_EULER_GAMMA, RealType::TheReal ) ) ;

	dict[ "and" ] = ValuePair( AND, new ANDNode() ) ; 
	dict[ "or"  ] = ValuePair( OR , new ORNode () ) ;
	dict[ "not" ] = ValuePair( NOT, new NOTNode() ) ;
	dict[ "xor" ] = ValuePair( XOR, new XORNode() ) ; 
	dict[ "imp" ] = ValuePair( IMP, new IMPNode() ) ; 
	dict[ "eqv" ] = ValuePair( EQV, new EQVNode() ) ; 

	dict[ "="  ] = ValuePair( OP_EQ, new EqualNode() ) ;
	dict[ "!=" ] = ValuePair( OP_EQ, new NotEqualNode() ) ;

	dict[ "<=" ] = ValuePair( OP_REL, new LessEqualNode() ) ;
	dict[ ">=" ] = ValuePair( OP_REL, new GreaterEqualNode() ) ;
	dict[ "<"  ] = ValuePair( OP_REL, new LessNode() ) ;
	dict[ ">"  ] = ValuePair( OP_REL, new GreaterNode() ) ;

	dict[ ":=" ] = ValuePair( OP_ASSIGN,  new AssignNode() ) ;
	dict[ "+"  ] = ValuePair( OP_ADD_SUB, new PlusNode() ) ;
	dict[ "-"  ] = ValuePair( OP_ADD_SUB, new MinusNode() ) ;
	dict[ "/"  ] = ValuePair( OP_MUL_DIV, new DividesNode() ) ;
	dict[ "*"  ] = ValuePair( OP_MUL_DIV, new MultipliesNode() ) ;

	dict[ "truecount"  ] = ValuePair( COUNT, new CountNode( Real::one ) ) ;
	dict[ "falsecount" ] = ValuePair( COUNT, new CountNode( Real::zero ) ) ;
	dict[ "undefcount" ] = ValuePair( COUNT, new CountNode( Real::tundef ) ) ;

	dict[ "even" ] = ValuePair( COND_REAL_FUNC, new FuncEVEN() ) ; 
	dict[ "odd" ] = ValuePair( COND_REAL_FUNC, new FuncODD() ) ; 
	dict[ "isint" ] = ValuePair( COND_REAL_FUNC, new FuncISINT() ) ; 
	dict[ "isprime" ] = ValuePair( COND_REAL_FUNC, new FuncISPRIME() ) ; 
	dict[ "isundefined" ] = ValuePair( COND_REAL_FUNC, new FuncISUNDEFINED() ) ;

	dict[ "tan" ] = ValuePair( UNARY_FUNC, new FuncTAN() );
	dict[ "tanh" ] = ValuePair( UNARY_FUNC, new FuncTANH() );
	dict[ "sqrt" ] = ValuePair( UNARY_FUNC, new FuncSQRT() );
	dict[ "sinh" ] = ValuePair( UNARY_FUNC, new FuncSINH() );
	dict[ "sin" ] = ValuePair( UNARY_FUNC, new FuncSIN() );
	dict[ "round" ] = ValuePair( UNARY_FUNC, new FuncROUND() );
	dict[ "fractional" ] = ValuePair( UNARY_FUNC, new FuncFRACTIONAL() );
	dict[ "remainder" ] = ValuePair( BINARY_FUNC, new FuncREMAINDER() );
	dict[ "abs" ] = ValuePair( UNARY_FUNC, new FuncABS() );
	dict[ "exp" ] = ValuePair( UNARY_FUNC, new FuncEXP() );
	dict[ "ln" ] = ValuePair( UNARY_FUNC, new FuncLN() );
	dict[ "log" ] = ValuePair( UNARY_FUNC, new FuncLOG() );
	dict[ "power" ] = ValuePair( BINARY_FUNC, new FuncPOWER() ) ;
	dict[ "cosh" ] = ValuePair( UNARY_FUNC, new FuncCOSH() );
	dict[ "cos" ] = ValuePair( UNARY_FUNC, new FuncCOS() );
	dict[ "sec" ] = ValuePair( UNARY_FUNC, new FuncSEC() );
	dict[ "sech" ] = ValuePair( UNARY_FUNC, new FuncSECH() );
	dict[ "trunc" ] = ValuePair( UNARY_FUNC, new FuncTRUNC() );
	dict[ "truncupper" ] = ValuePair( UNARY_FUNC, new FuncTRUNCUPPER() );
	dict[ "atanh" ] = ValuePair( UNARY_FUNC, new FuncATANH() );
	dict[ "atan" ] = ValuePair( UNARY_FUNC, new FuncATAN() );
	dict[ "asinh" ] = ValuePair( UNARY_FUNC, new FuncASINH() );
	dict[ "asin" ] = ValuePair( UNARY_FUNC, new FuncASIN() );
	dict[ "acosh" ] = ValuePair( UNARY_FUNC, new FuncACOSH() );
	dict[ "acos" ] = ValuePair( UNARY_FUNC, new FuncACOS() );
	dict[ "sign" ] = ValuePair( UNARY_FUNC, new FuncSIGN() );
	dict[ "fact" ] = ValuePair( UNARY_FUNC, new FuncFACT() );
	dict[ "max" ] = ValuePair( BINARY_FUNC, new FuncMAX() ) ;
	dict[ "min" ] = ValuePair( BINARY_FUNC, new FuncMIN() ) ;
	dict[ "logn" ] = ValuePair( BINARY_FUNC, new FuncLOGN() ) ;
	dict[ "root" ] = ValuePair( BINARY_FUNC, new FuncROOT() ) ;
	dict[ "comb" ] = ValuePair( BINARY_FUNC, new FuncCOMB() ) ;
	dict[ "lcm" ] = ValuePair( BINARY_FUNC, new FuncMCM() ) ;
	dict[ "gcd" ] = ValuePair( BINARY_FUNC, new FuncGCD() ) ;
	dict[ "hip" ] = ValuePair( BINARY_FUNC, new FuncHIP() ) ;
	dict[ "recttopolar_angle" ] = ValuePair( BINARY_FUNC, new FuncRECTTOPOLAR_ANGLE() ) ;
	dict[ "recttopolar_r" ] = ValuePair( BINARY_FUNC, new FuncRECTTOPOLAR_R() ) ;
	dict[ "polartorect_x" ] = ValuePair( BINARY_FUNC, new FuncPOLARTORECT_X() ) ;
	dict[ "polartorect_y" ] = ValuePair( BINARY_FUNC, new FuncPOLARTORECT_Y() ) ;
	dict[ "cotan" ] = ValuePair( UNARY_FUNC, new FuncCOTAN() );
	dict[ "cosec" ] = ValuePair( UNARY_FUNC, new FuncCOSEC() );
	dict[ "cosech" ] = ValuePair( UNARY_FUNC, new FuncCOSECH() );
	dict[ "nextprime" ] = ValuePair( UNARY_FUNC, new FuncNEXTPRIME() );
	dict[ "radtodeg" ] = ValuePair( UNARY_FUNC, new FuncRADTODEG() );
	dict[ "degtorad" ] = ValuePair( UNARY_FUNC, new FuncDEGTORAD() );
	dict[ "nth_prime" ] = ValuePair( UNARY_FUNC, new FuncNTH_PRIME() );
	dict[ "asec" ] = ValuePair( UNARY_FUNC, new FuncASEC() );
	dict[ "acotan" ] = ValuePair( UNARY_FUNC, new FuncACOTAN() );
	dict[ "asech" ] = ValuePair( UNARY_FUNC, new FuncASECH() );
	dict[ "acosech" ] = ValuePair( UNARY_FUNC, new FuncACOSECH() );
 	dict[ "acotanh" ] = ValuePair( UNARY_FUNC, new FuncACOTANH() );
 	dict[ "ctok" ] = ValuePair( UNARY_FUNC, new FuncCTOK() );
 	dict[ "ctof" ] = ValuePair( UNARY_FUNC, new FuncCTOF() );
 	dict[ "ktoc" ] = ValuePair( UNARY_FUNC, new FuncKTOC() );
 	dict[ "ftoc" ] = ValuePair( UNARY_FUNC, new FuncFTOC() );
 	dict[ "ftok" ] = ValuePair( UNARY_FUNC, new FuncFTOK() );
 	dict[ "ktof" ] = ValuePair( UNARY_FUNC, new FuncKTOF() );

	// Random functions (with different distributions):
	dict[ "random" ] = ValuePair( WITHOUT_PARAM_FUNC_RANDOM, new FuncRANDOM() ) ;
	dict[ "beta" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncBETA() ) ;
	dict[ "chi" ] = ValuePair( UNARY_FUNC_RANDOM, new FuncCHI() ) ;
	dict[ "exponential" ] = ValuePair( UNARY_FUNC_RANDOM, new FuncEXPONENTIAL() ) ;
	dict[ "f" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncF() ) ;
	dict[ "gamma" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncGAMMA() ) ;
	dict[ "normal" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncNORMAL() ) ;
	dict[ "uniform" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncUNIFORM() ) ;
	dict[ "binomial" ] = ValuePair( BINARY_FUNC_RANDOM, new FuncBINOMIAL() ) ;
	dict[ "poisson" ] = ValuePair( UNARY_FUNC_RANDOM, new FuncPOISSON() ) ;
	dict[ "randint" ] = ValuePair( UNARY_FUNC_RANDOM, new FuncRANDINT() ) ;
	dict[ "randomsign" ] = ValuePair( WITHOUT_PARAM_FUNC_RANDOM, new FuncRANDOMSIGN() ) ;

	dict[ "if" ] = ValuePair( COND3_FUNC, new IFNode() ) ; 
	dict[ "ifu" ] = ValuePair( COND4_FUNC, new IFUNode() ) ; 
} ;

/*******************************************************************
* Method: ~Parser
* Description: Destructor
********************************************************************/
Parser::~Parser()
{
	for ( Dictionary::iterator dcur = dict.begin(); dcur != dict.end(); dcur++ )
		delete dcur->second.second;

	if (this->cursor)
		delete cursor;
}

/*******************************************************************
* Method: analizeToken
********************************************************************/
int Parser::analizeToken( string &token, bool &consumed, string &text )
{
	string::iterator curs( token.begin() );
	register int value( curs == token.end() ? 0 : *curs );

	if( curs != token.end() )
	{
		switch( *curs )
		{
			case '(':
			case ')':
			case '[':
			case ']':
			case ';':
			case ',':
			case '{':
			case '}':
				value = *curs ;
				curs++ ;
				break;

			case '-':
				// Unary negate ( for a number)
				curs++ ;
				
				// Veo si lo que sigue es la palabra INF
				if (curs != token.end() && *curs == 'i'){
					curs++;
					if (curs != token.end() && *curs == 'n'){
						curs++;
						if (curs != token.end() && *curs == 'f'){
							curs++;
							value = 0;
							break;
						}
					}
					value = 0;
					break;
				}

				if( curs == token.end() || (*curs != '.' && *curs < '0' && *curs > '9' ))
				{
					value = OP_ADD_SUB;
					break;
				}

				// Sino veo si lo que sigue es de tipo: .ddddd
				// (con d un digito):
				if (*curs == '.')
				{
					curs++;		// Avanzo el '.'
					IsDigitPredicate pred0 ;
					while( curs != token.end() && pred0( *curs ) ) curs++ ;
					value = REAL;
					break;
				}
				// Sino veo si lo que sigue es de tipo ddd[.ddd]

			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				// avanza el cursor mienstras sean numeros
				// isDigit( curs, token.end() ) ;
				{
					IsDigitPredicate pred ;
					while( curs != token.end() && pred( *curs ) ) curs++ ;
				}
				value = INT ;

				if (*curs == '.') // Veo si tiene parte decimal (es REAL)
				{
					curs++;		// Avanzo el '.'
					IsDigitPredicate pred2 ;
					while( curs != token.end() && pred2( *curs ) ) curs++ ;

					value = REAL;
				}
				break; 

			case '.':
				{
					curs++;		// Avanzo el '.'
					IsDigitPredicate pred ;
					while( curs != token.end() && pred( *curs ) ) curs++ ;
					value = REAL;
				}
				break; 

			case '?':
				value = UNDEF ;
				curs++ ;
				break;

			case '+':
				curs++;
				value = OP_ADD_SUB;
				break;

			case '*':
			case '/':
				curs++;
				value = OP_MUL_DIV;
				break;

			case '!':
				curs++;
				if (curs != token.end() && *curs == '=') {
					curs++;
					value = OP_EQ;
				}
				break;

			case '>':
			case '<':
				curs++;
				if (curs != token.end() && *curs == '=')
					curs++;
				value = OP_REL;
				break;

			case '=':
				curs++;
				value = 0;
				break;
				
			case ':':
				curs++;
				value = 0;
				if (curs != token.end() && *curs == '=') {
					curs++;
					value = OP_ASSIGN;
				}
				break;
			
			case PORT_PREFIX:
			case VAR_PREFIX:
                                // At this point, variables and ports are treated
				// as strings with an invalid first character.
				curs++;
			default:
				// es un string, avanza el cursor mientras lo parsea
				while( curs != token.end() && esCharStrValido(*curs) )
					curs++ ;
				value = 0 ;
		}
	}

	consumed = ( curs == token.end() );

	text = token.substr( 0, curs - token.begin() );
	token = token.substr( curs - token.begin() );

	if (text == "statecount")
		value = STATECOUNT;
	else if (text == "time")
		value = WITHOUT_PARAM_FUNC_TIME;
	else if (text == "portvalue")
		value = PORTREF;
	else if (text == "send") {
		value = SEND;
		setTempParsingForPortIn();
	} else if (text == "cellpos")
		value = CELLPOS;
	else if (text.rfind(VAR_PREFIX) == 0) {
		text.erase(0, sizeof(VAR_PREFIX));
		value = STVAR_NAME;
	} else if (text.rfind(PORT_PREFIX) == 0) {
		text.erase(0, sizeof(PORT_PREFIX));
		value = 0;
		setTempParsingForPortIn();
	}

	
	return value;
} 

void printToken(string &t, int tipo)
{
	switch(tipo)
	{
		case OP_ASSIGN:		ParserDebug().Stream() << "OP_ASSIGN parsed\n";
					break;
		case OP_ADD_SUB:	ParserDebug().Stream() << "OP_ADD_SUB parsed ("<< t << ")\n";
					break;
		case OP_MUL_DIV:	ParserDebug().Stream() << "OP_MUL_DIV parsed ("<< t << ")\n";
					break;
		case OP_EQ:		ParserDebug().Stream() << "OP_EQ parsed ("<< t << ")\n";
					break;
		case OP_REL:		ParserDebug().Stream() << "OP_REL parsed ("<< t << ")\n";
					break;
		case COUNT:		ParserDebug().Stream() << "COUNT parsed ("<< t << ")\n";
					break;
		case AND:		ParserDebug().Stream() << "AND parsed\n";
					break;
		case OR:		ParserDebug().Stream() << "OR parsed\n";
					break;
		case NOT:		ParserDebug().Stream() << "NOT parsed\n";
					break;
		case XOR:		ParserDebug().Stream() << "XOR parsed\n";
					break;
		case IMP:		ParserDebug().Stream() << "IMP parsed\n";
					break;
		case EQV:		ParserDebug().Stream() << "EQV parsed\n";
					break;
		case UNDEF:		ParserDebug().Stream() << "UNDEF parsed (?)\n";
					break;
		case BOOL:		ParserDebug().Stream() << "BOOL parsed ("<< t << ")\n";
					break;
		case UNARY_FUNC_RANDOM:
		case UNARY_FUNC:	ParserDebug().Stream() << "UNARY_FUNCTION " << t << " parsed\n";
					break;
		case WITHOUT_PARAM_FUNC_TIME:
		case WITHOUT_PARAM_FUNC_RANDOM:
					ParserDebug().Stream() << "Function without param. (" << t << ") parsed\n";
					break;
		case COND_REAL_FUNC:	ParserDebug().Stream() << "COND_REAL_FUNC " << t << " parsed\n";
					break;
		case BINARY_FUNC_RANDOM:
		case BINARY_FUNC:	ParserDebug().Stream() << "BINARY_FUNCTION " << t << " parsed\n";
					break;
		case CONSTFUNC:		ParserDebug().Stream() << "Constant (" << t << ") parsed\n";
					break;
		case COND3_FUNC:	ParserDebug().Stream() << "Conditional IF function parsed\n";
					break;
		case COND4_FUNC:	ParserDebug().Stream() << "Conditional IFU function parsed\n";
					break;
		case 0:			ParserDebug().Stream() << "Undefined token parsed ("<< t << ")\n";
					break;
	}
}

/*******************************************************************
* Method: nextToken
********************************************************************/
int Parser::nextToken()
{
	register int value( 0 ) ; /* token end */

	while( this->read && *cursor != TokenIterator() && isBlank( (*cursor)->begin(), (*cursor)->end() ) )
		(*cursor)++ ;

	if( !read || *cursor != TokenIterator() )
	{
		if( read )
		{
			token = *(*cursor);
			(*cursor)++ ;
			token = lowerCase( token );
		}
		string text;

		value = analizeToken( this->token, this->read, text );

		switch( value )
		{
			case REAL:
			case INT:
				{
					Real number = str2Real( text.c_str() ) ;
					yylval.PSyntaxNode.Node = new ConstantNode( number, RealType::TheReal ) ;

					if (this->printParserInfo)
						ParserDebug().Stream() << "Number " << number << " analyzed\n";
				}
				break;

			case OP_ASSIGN:
			case OP_ADD_SUB:
			case OP_MUL_DIV:
			case OP_REL:
			case OP_EQ:
			case UNDEF:
			case BOOL:
			case 0:
				{
					Dictionary::iterator iter = dict.find( text ) ;
		
					if( iter != dict.end() )
					{
						yylval.PSyntaxNode.Node = (*iter).second.second->clone() ;
						value = (*iter).second.first ;
					}
					else if (this->parsingForPortIn()){
						// may be a portName
						string pName = lowerCase( text.c_str() );

						if (this->printParserInfo)
							ParserDebug().Stream() << "String '" << pName << "' parsed.\n";

						yylval.PSyntaxNode.Node = new StringNode(pName) ; 
						value = PORTNAME;
						setOldParsingForPortIn();
					}
					else if (this->printParserInfo)
						ParserDebug().Stream() << "Token %" << token << "% text %" << text << "% not found in dict!" << endl ;

					if (this->printParserInfo)
						printToken(text, value);
				}
				break;
				
			case STATECOUNT:
				{
					if (this->printParserInfo)
						ParserDebug().Stream() << "STATECOUNT parsed (" << text << ")\n";
				}
				break;	// No hace falta hacer nada.

			case WITHOUT_PARAM_FUNC_TIME:
				{
					if (this->printParserInfo)
						ParserDebug().Stream() << "Function without parameter parsed (" << text << ")\n";
				}
				break;	// No hace falta hacer nada.

			case PORTREF:
				{
					MASSERTMSG( this->parsingForPortIn(), "The function PortValue only can be used within a portInTransition declaration." );
					if (this->printParserInfo)
						ParserDebug().Stream() << "Function PORTVALUE parsed.\n";
				}
				break;

			case SEND:
				{
					if (this->printParserInfo)
						ParserDebug().Stream() << "Function SEND parsed.\n";
				}
				break;

			case CELLPOS:
				{
					if (this->printParserInfo)
						ParserDebug().Stream() << "Function CELLPOS parsed.\n";
				}
				break;
			case STVAR_NAME:
				{
					yylval.PSyntaxNode.Node = new StateVarNode(text);
					
					if (this->printParserInfo)
						ParserDebug().Stream() << "State Variable " << text << " parsed.\n";
				}
				break;
		}
	}
	return value;
}

/*******************************************************************
* Method: parse
********************************************************************/
Parser &Parser::parse( istream &is, bool printParserInformation, bool parseForPortIn, const string &elseFunction )
{
	reset();
	parsingForPortIn( parseForPortIn );

	read = true ;
	printParserInfo = printParserInformation ;

	if( cursor )
		delete cursor ;

	cursor = new TokenIterator( is ) ;

	if( yyparse() )
	{
		ErrorParsingException e ;
		MTHROW( e ) ;
	}
	spec->elseFunction( elseFunction );
	return *this ;
} 

/*******************************************************************
* Method: error
********************************************************************/
int Parser::error( const string &errText )
{
	cout << "Parsing error " << errText << endl ;
	return 1 ;
}

/*******************************************************************
* Method: addRule
********************************************************************/
Parser &Parser::addRule( RuleNode *rn, int Stochastic )
{
	if( !spec )
		spec = new SpecNode() ;

	this->spec->addRule( rn, Stochastic ) ;

	return *this ;
}

/*******************************************************************
* Function Name: reset
* Description: just deletes the list, not the elements!
********************************************************************/
Parser &Parser::reset()
{
	// WARNING!!!! The spec var is allocated dynamically, it must 
	// be dealocated outside the parser ( LocalTransitionAdmin )

	spec = NULL ;
	return *this ;
}

// ** lex functions ** //
int yyerror( const char *s )
{
	SingleParser::Instance().error( s ) ;
	return 1 ;
}

int yylex()
{
	return SingleParser::Instance().nextToken() ;
} ;


///////////////////////////////////
// ** main method **            //
/////////////////////////////////
/*
#include "single.h"

int main( int, char *argv[] ) {

	ifstream file( argv[ 1 ] ) ;

	try {
		SingleParser::Instance().parse( file ) ;

//		Parser::RuleList *rules( SingleParser::Instance().getRules() ) ;
//		Parser::RuleList::iterator curs ;

//		for( curs = rules->begin() ; curs != rules->end() ; curs++  )
//			ParserDebug().Stream() << "Rule : " << **curs << endl ;

	} catch( MException &e ) {
		cerr << e ;
	} catch( ... ) {
		cerr << "Unknown exception thrown!" << endl ;
	}
}
*/
