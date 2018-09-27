/*******************************************************************
*
*  DESCRIPTION: class StateVars
*
*  AUTHOR: Alejandro López
*
*  EMAIL: mailto://alopez@dc.uba.ar
*         
*
*  REVISION: $Revision$  $Date$
*
*******************************************************************/

#ifndef __STATEVARS_H
#define __STATEVARS_H

#include <string>
#include <utility>
#include <map>

#include "except.h"
#include "real.h"
#include "value.h"

class StateVarsException : public MException
{
    public:
    StateVarsException(const std::string &str = "Undefined state variable"): 
                MException(str)  {} ;
};

class StateVars : protected std::map<const std::string, value_ptr>
{
    public:

    inline bool exist(const std::string& name) const
    { return (find(name) != end()); }
    
    bool createVariable(const std::string& name, AbstractValue& value = Real::tundef);

    AbstractValue &get(const std::string &name) const;           //throws StateVarsException
    AbstractValue &set(const std::string &name, AbstractValue &newValue); //throws StateVarsException
    
    StateVars& operator=(const StateVars &src);
    
    StateVars& setValues(const std::string &values);

    std::string asString(void) const;
    
    inline void print(std::ostream& os) const
    { os << asString(); }
    
    void clear()
    { order.clear(); this->std::map<const std::string, value_ptr>::clear(); }
    
    
    protected:
    
    inline bool exist(int index) const
    {
    	std::map<int, std::string>::const_iterator it = order.find(index);
    	return (it != order.end());
    }
    
    const std::string& operator[](int index) const;

    private:
    std::map<int, std::string> order;
};



#endif // __STATEVARS_H
