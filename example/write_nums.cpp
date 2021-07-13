#include "gulachek/gtree.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"

#include <iostream>
#include <vector>

namespace gt = gulachek::gtree;

int main()
{
	std::vector<std::size_t> ns = { 1, 2, 3, 256, 257, 258 };

	gt::write(gt::tout, ns);

	return 0;
}
