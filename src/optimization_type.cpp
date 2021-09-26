#include "gulachek/gtree/encoding/optimization_type.hpp"

namespace gulachek::gtree
{
	std::ostream& operator << (std::ostream &os, optimization_type ot)
	{
		switch (ot)
		{
			case optimization_type::empty: return os << "empty";
			case optimization_type::value: return os << "value";
			case optimization_type::container: return os << "container";
			case optimization_type::hybrid: return os << "hybrid";
			default: return os << "(invalid)";
		}
	}
}
