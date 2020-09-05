#include <gulachek/gtree.hpp>

#include <string>
#include <iostream>

namespace gt = gulachek::gtree;

struct NumTree
{
	std::size_t _n;

	const gt::block value() const
	{ return {&_n, sizeof(_n)}; }

	std::size_t child_count() const
	{ return 0; }

	NumTree child(std::size_t i) const
	{ return {}; }
};

struct NumListTree
{
	std::vector<std::size_t> _nums;

	const gt::block value() const
	{ return {}; }

	std::size_t child_count() const
	{ return _nums.size(); }

	NumTree child(std::size_t i) const
	{ return NumTree{_nums[i]}; }
};

int main()
{
	NumListTree t;

	std::size_t n;
	while (std::cin >> n)
		t._nums.push_back(n);

	gt::tout.write(t);

	std::exit(EXIT_SUCCESS);
}
