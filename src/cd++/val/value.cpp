#include <sstream>

#include "value.h"
#include "real.h"
#include "tuple_value.h"


std::ostream &operator<<(std::ostream &os, const AbstractValue &val)
{
	val.print(os);
	return os;
}

TValBool operator!=(const AbstractValue &val1, const AbstractValue &val2)
{
	return !(val1 == val2);
}

TValBool operator==(const AbstractValue &val1, const AbstractValue &val2)
{
	return typeid(val1) == typeid(val2) && val1.equals(val2);
}

value_ptr AbstractValue::from_string(const std::string& str)
{
	std::stringstream stream(str);
	Real r;
	Tuple<Real> t;

	if(stream >> t)
		return AbstractValue::to_value_ptr(t);
	else
	{
		stream.clear();
		if(stream >> r)
			return AbstractValue::to_value_ptr(r);
		else
		{
			MException e;
			MTHROW(e);
		}
	}
}

value_ptr AbstractValue::to_value_ptr(const AbstractValue& val)
{
	return val.clone();
}

value_ptr AbstractValue::to_value_ptr(const double& val)
{
	Real r(val);
	return dynamic_cast<AbstractValue*>(&r)->clone();
}
