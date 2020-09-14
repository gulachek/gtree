#include "gulachek/gtree.hpp"
#include <fstream>
#include <iostream>

#include <boost/fusion/include/adapt_struct.hpp>

namespace gt = gulachek::gtree;

struct Foo
{
	std::size_t bar;
	std::string baz;
};

BOOST_FUSION_ADAPT_STRUCT(Foo, bar, baz);

std::ostream& operator << (std::ostream& os, const Foo &foo)
{
	return os << "Foo{" << foo.bar << ',' << foo.baz << '}';
}

int main(int argc, char **argv)
{
	if (argc != 3 ||
			!(argv[1] == std::string{"r"} ||
				argv[1] == std::string{"w"}))
	{
		std::cerr << "Usage: " << argv[0] << " <r|w> <file> " << std::endl;
		return 1;
	}

	if (argv[1][0] == 'w')
	{
		std::ofstream of{argv[2]};
		gt::otreem tout{of};

		Foo foo{654321, "goodbye earth"};
		tout << foo;
	}
	else
	{
		std::ifstream inf{argv[2]};
		gt::itreem tin{inf};

		Foo foo;
		tin >> foo;

		std::cout << foo << std::endl;
	}

	return 0;
}
