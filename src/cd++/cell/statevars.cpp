/*******************************************************************
*
*  DESCRIPTION: class StateVarsList
*
*  AUTHOR: Alejandro López
*
*  EMAIL: mailto://alopez@dc.uba.ar
*         
*
*  REVISION: $Revision$  $Date$
*
*******************************************************************/
#include <cstdlib>
#include <string>
#include "real.h"
#include "statevars.h"

using namespace std;

bool StateVars::createVariable(const string& name, AbstractValue& value)
{
    if (exist(name))
        return false;
    
    map<const string, value_ptr>::operator[](name) = AbstractValue::to_value_ptr(value);
    order[order.size()] = name;
    return true;
}

AbstractValue &StateVars::get(const string &name) const
{
    if (!exist(name)) {
        StateVarsException e("Undefined state variable '" + name + "'");
        MTHROW(e);
    }


    return *(const_cast<StateVars *>(this)->map<const string, value_ptr>::operator[](name));
}

AbstractValue &StateVars::set(const string &name, AbstractValue &newValue)
{
    if (!exist(name)) {
        StateVarsException e("Undefined state variable '" + name + "'");
        MTHROW(e);
    }

    value_ptr ptr(&newValue);
    this->map<const string, value_ptr>::operator[](name) = ptr;
    return *(this->map<const string, value_ptr>::operator[](name));
}


StateVars& StateVars::operator=(const StateVars &src)
{
    clear();
    
/*
    for (StateVars::const_iterator var = src.begin(); var != src.end(); var++) {
        Real value = var->second;
        string name(var->first);
        createVariable(name, value);
    }
*/

    for (int idx = 0; idx < (int) src.size(); idx++) {
        string name(src[idx]);
        AbstractValue &value = src.get(name);
        createVariable(name, value);
    }

    return *this;
}

StateVars& StateVars::setValues(const string &values)
{
    int idx = 0;
    string::const_iterator cursor = values.begin();
    
    while (idx < (int) this->order.size() && cursor != values.end()) {
    	// skip spaces
    	while (cursor != values.end() && isspace(*cursor))
	    cursor++;
	    
    	// read number
	string value = "";
	while (cursor != values.end() && !isspace(*cursor)) {
    	    value += *cursor;
	    cursor++;
	}

	if (!value.empty()) {
            // set the value
            Real numval(strtod(value.c_str(), NULL));
            set(order[idx], numval);

            // prepare for next loop
            idx++;
        }
    }

    // skip last spaces
    while (cursor != values.end() && isspace(*cursor))
	cursor++;

    // check that both loop have finished
    if (cursor != values.end() || idx != (int) this->order.size()) {
        StateVarsException e("Incorrect number of initial values for state variables");
        MTHROW(e);
    }

    return *this;
}

string StateVars::asString(void) const
{
    string res;
    StateVars::const_iterator name = begin();
    
    res = "(";
    while (name != end()) {
        res += (name->first + " = " + name->second->asString());
        name++;
        if (name != end())
            res += ", ";
    }
    res += ")";
    
    return res;
}

const string& StateVars::operator[](int index) const
{
    if (!exist(index)) {
        StateVarsException e("Undefined state variable index <" + Real(index).asString() + ">");
        MTHROW(e);
    }
    
    return const_cast<StateVars *>(this)->order[index];
}

