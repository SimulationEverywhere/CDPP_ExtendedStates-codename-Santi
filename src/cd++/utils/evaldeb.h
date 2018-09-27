/*******************************************************************
*
*  DESCRIPTION: CLASS EvalDebug
*
*  AUTHOR:    Daniel Rodriguez.
*  VERSION 2: Gabriel Wainer.
*
*  EMAIL: mailto://drodrigu@dc.uba.ar
*  EMAIL: mailto://gabrielw@dc.uba.ar
*
*  DATE: 20/10/2000
*  DATE: 15/08/2000
*	strat in-class inicialization removed to make code ANSI C++
*	compatible
*
*******************************************************************/

#ifndef __EVAL_PARAMS_H
#define __EVAL_PARAMS_H

/** definitions **/
#include <ostream>
#include <cstdlib>

class clsEvalParam
{
public:
	clsEvalParam()				//Default constructor
		:evalParamOut(NULL)
		,evalParamActive(false)
		,strat(false)
                {}

	std::ostream &Stream()
			{return *evalParamOut;}
			
	void Stream( std::ostream *evalParamStr )
			{ evalParamOut = evalParamStr; }

	const bool Active()
			{return evalParamActive;}
			
	void Active( bool mode )
			{ evalParamActive = mode; }

        const bool Strat()  //GW. Strategy 1: false. 2: true.
                        {return strat;}
			
        void Strat ( bool mode )  //GW. Strategy 1: false. 2: true.
                        { strat = mode; }

	double Value( void )
			{ return value; }

	void Value( double valor )
			{ value = valor; }

        double Ratio ( void )    // GW. Returns the ratio used with dyn. q
                        { return ratio; }

        void Ratio( double percent )
                        { ratio = percent; }

protected:
	// ** Instance variables ** //
	std::ostream *evalParamOut ;
	bool evalParamActive ;
	double value ;
        double ratio;
        bool strat;

};	// class EvalParam




extern clsEvalParam	*evalDebugInstance;
extern clsEvalParam	*flatDebugInstance;
extern clsEvalParam	*rulesDebugInstance;
extern clsEvalParam	*parserDebugInstance;
extern clsEvalParam	*showVirtualTimeDebugInstance;
extern clsEvalParam	*useQuantumInstance;
extern clsEvalParam     *useDynQuantumInstance;



inline clsEvalParam &ShowVirtualTimeWhenFinish()
{
	if (showVirtualTimeDebugInstance == NULL)
		showVirtualTimeDebugInstance = new clsEvalParam;
	
	return *showVirtualTimeDebugInstance;
}	

inline clsEvalParam &EvalDebug()
{
	if (evalDebugInstance == NULL)
		evalDebugInstance = new clsEvalParam;
	
	return *evalDebugInstance;
}	

inline clsEvalParam &FlatDebug()
{
	if (flatDebugInstance == NULL)
		flatDebugInstance = new clsEvalParam;
	
	return *flatDebugInstance;
}	

inline clsEvalParam &DebugCellRules()
{
	if (rulesDebugInstance == NULL)
		rulesDebugInstance = new clsEvalParam;
	
	return *rulesDebugInstance;
}	

inline clsEvalParam &ParserDebug()
{
	if (parserDebugInstance == NULL)
		parserDebugInstance = new clsEvalParam;
	
	return *parserDebugInstance;
}	

inline clsEvalParam &UseQuantum()
{
	if (useQuantumInstance == NULL)
		useQuantumInstance = new clsEvalParam;
	
	return *useQuantumInstance;
}	

inline clsEvalParam &UseDynQuantum()  // Dynamic Quantizer
{
        if (useDynQuantumInstance == NULL)
                useDynQuantumInstance = new clsEvalParam;
	
        return *useDynQuantumInstance;
}	

#endif   //__EVAL_PARAMS_H 
