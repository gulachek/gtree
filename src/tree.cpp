#include "gulachek/gtree/tree.hpp"

using namespace gulachek::gtree;

tree tree::child(std::size_t index) const
{
#ifndef NDEBUG
	if (child_count() <= index)
		throw std::out_of_range{"Child index out of bounds"};
#endif

	return tree{_base, _node.first_child + index};
}

tree::tree(
		const std::shared_ptr<tree_base> &base,
		std::size_t index
		) : _base{base}
{
#ifndef NDEBUG
	if (base->nodes.size() <= index)
		throw std::out_of_range{"Node index out of bounds"};
#endif

	 _node = base->nodes[index];
}

tree::tree() :
	_base{nullptr}
{
	_node = node{block{}, 0, 0};
}
