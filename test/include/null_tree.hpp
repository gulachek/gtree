#ifndef NULL_TREE_HPP
#define NULL_TREE_HPP

#include "gulachek/gtree.hpp"

struct NullTree
{
	const gulachek::gtree::block value() const { return {}; }

	std::size_t child_count() const { return 0; }

	NullTree child(std::size_t i) const { return {}; }
};

#endif
