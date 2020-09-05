#include "gulachek/gtree.hpp"

#include <iostream>
#include <vector>

namespace gt = gulachek::gtree;

int main()
{
	std::vector<std::size_t> ns = { 1, 2, 3, 256, 257, 258 };
	gt::tout << ns;

	return 0;
}
