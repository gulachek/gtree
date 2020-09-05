#include "gulachek/gtree.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace gt = gulachek::gtree;

static std::string _hex(const gt::block &b)
{
	std::stringstream ss;
	ss << std::hex;
	for (auto i = 0; i < b.size(); i++)
		ss << std::setfill('0') << std::setw(2) << (int)b[i];

	return ss.str();
}

void _print(const gt::tree &t, std::size_t ident)
{
	std::cout << std::string(ident, ' ') <<
			'<' << _hex(t.value()) << '>' << std::endl;

	for (auto i = 0; i < t.child_count(); i++)
		_print(t.child(i), ident+1);
}

template <typename ITreem>
bool gtree2hex(ITreem &itreem)
{
	gt::tree t;

	try
	{
		while (itreem.read(&t))
				_print(t, 0);
		return true;
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return false;
	}
}

int main(int argc, char** argv)
{
	bool success = true;

	if (argc == 1)
	{
		success = gtree2hex(gt::tin);
	}
	else
	{
		for (std::size_t i = 1; i < argc; i++)
		{
			std::ifstream ifs{argv[i]};
			if (ifs)
			{
				gt::itreem it{ifs};
				success = success && gtree2hex(it);
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
