#include "gulachek/gtree.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <span>

namespace gt = gulachek::gtree;

static std::string _hex(const std::span<const std::uint8_t> &b)
{
	std::stringstream ss;
	ss << std::hex;
	for (std::size_t i = 0; i < b.size(); i++)
		ss << std::setfill('0') << std::setw(2) << (int)b[i];

	return ss.str();
}

void _print(const gt::tree &t, std::size_t ident)
{
	std::cout << std::string(ident, ' ') <<
			'<' << _hex(t.value()) << '>' << std::endl;

	for (std::size_t i = 0; i < t.child_count(); i++)
		_print(t.child(i), ident+1);
}

bool gtree2hex(std::istream &is)
{
	gt::tree t;

	while (true)
	{
		if (auto err = gt::read(is, &t))
		{
			if (err.is_eof())
				return true;

			std::cerr << err << std::endl;
			return false;
		}

		_print(t, 0);
	}
}

int main(int argc, char** argv)
{
	bool success = true;

	if (argc == 1)
	{
		success = gtree2hex(std::cin);
	}
	else
	{
		for (int i = 1; i < argc; i++)
		{
			std::ifstream ifs{argv[i]};
			if (ifs)
			{
				success = success && gtree2hex(ifs);
			}
			else
			{
				std::cerr << "Failed to open file " << argv[i] << std::endl;
				success = false;
			}
		}
	}

	std::exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}
