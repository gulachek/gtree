#include "gulachek/gtree.hpp"
#include <fstream>
#include <iostream>

namespace gt = gulachek::gtree;

class Foo
{
	public:
		typedef std::tuple<std::size_t, std::string> gtree_encoding;

		Foo() = default;

		static gtree_encoding gtree_encode(const Foo &foo)
		{
			return {foo._bar, foo._baz};
		}

		static Foo gtree_decode(gtree_encoding &&encoding)
		{
			return Foo{
				std::move(std::get<0>(encoding)),
				std::move(std::get<1>(encoding))
			};
		}

		Foo(std::size_t bar, const std::string &baz) :
			_bar{bar}, _baz{baz}
		{}

		std::size_t bar() const { return _bar; }
		std::string baz() const { return _baz; }

	private:
		std::size_t _bar;
		std::string _baz;
};

std::ostream& operator << (std::ostream& os, const Foo &foo)
{
	return os << "Foo{" << foo.bar() << ',' << foo.baz() << '}';
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

		Foo foo{123456, "hello world"};
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
