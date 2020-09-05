#include "gulachek/gtree.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>

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

int main()
{
	gt::tree t;
	while (gt::tin.read(&t))
			_print(t, 0);

	return 0;
}
