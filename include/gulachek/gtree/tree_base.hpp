#ifndef GULACHEK_GTREE_TREE_BASE_HPP
#define GULACHEK_GTREE_TREE_BASE_HPP

#include "gulachek/gtree/node.hpp"
#include <memory>
#include <cstdint>
#include <vector>

namespace gulachek::gtree
{
	/*
	 * Data structure representing an entire tree.
	 *
	 * It's expected that the consumer will manage
	 * the nodes to point at the correct location
	 * in the data block.  The sum of data sizes in
	 * the nodes should be equal to the total size
	 * of the data block.  This can be more quickly
	 * computed as the offset of the last node plus
	 * its size.
	 */
	struct tree_base
	{
		std::vector<node> nodes;
		std::vector<std::uint8_t> data;
	};
}

#endif
