#ifndef GULACHEK_GTREE_NODE_HPP
#define GULACHEK_GTREE_NODE_HPP

#include "gulachek/gtree/block.hpp"
#include <cstdlib>

namespace gulachek::gtree
{
	struct node
	{
		// Arbitrary value of node
		block value;

		// Index to first child in tree
		std::size_t first_child;

		// Number of children
		std::size_t child_count;
	};
}

#endif
