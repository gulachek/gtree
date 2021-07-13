#include "gulachek/gtree/error.hpp"

namespace gulachek::gtree
{
	error::operator bool() const
	{
		return !msg_.str().empty();
	}

	void error::format(std::ostream &os) const
	{
		os << msg_.str();
	}

	std::ostream& operator <<(std::ostream &os, const error &err)
	{
		err.format(os);
		return os;
	}
}
