#include "gulachek/cause.hpp"

namespace gulachek
{
	void cause::output(std::ostream &os) const
	{
		if (is_eof())
		{
			os << "eof";
			return;
		}

		os << ss_.str();
		if (!causes_.empty())
		{
			os << " : ";
			os << causes_[0];
		}
	}

	std::ostream& operator << (std::ostream &os, const cause &c)
	{
		c.output(os);
		return os;
	}
}
