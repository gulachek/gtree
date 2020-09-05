#include <iostream>
#include <variant>
#include <string>

#include "gulachek/gtree.hpp"

namespace gt = gulachek::gtree;

int main()
{
	std::variant<std::string, unsigned int> val;
	while (gt::tin >> val)
	{
		if (auto pint = std::get_if<unsigned int>(&val))
		{
			std::cout << "Received integer: " << *pint << std::endl;
		}
		else if (auto pstr = std::get_if<std::string>(&val))
		{
			std::cout << "Received string: " << *pstr << std::endl;
		}
	}

	return 0;
}
