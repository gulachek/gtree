#include <gulachek/gtree.hpp>
#include <gulachek/gtree/encoding/vector.hpp>
#include <gulachek/gtree/encoding/unsigned.hpp>

#include <vector>
#include <string>
#include <iostream>

namespace gt = gulachek::gtree;

int main()
{
	std::vector<std::size_t> v;

	std::size_t n;
	while (std::cin >> n)
		v.push_back(n);

	gt::write(std::cout, v);
	std::exit(EXIT_SUCCESS);
}
