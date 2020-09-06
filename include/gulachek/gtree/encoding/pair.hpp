#ifndef GULACHEK_GTREE_ENCODING_PAIR_HPP
#define GULACHEK_GTREE_ENCODING_PAIR_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	template <typename Tree, typename T, typename U>
	void decode(const Tree &tree, std::pair<T, U> &pair)
	{
		if constexpr (is_pure_value<T>::value)
		{
			decode(tree, pair.first);
			if constexpr (is_pure_container<U>::value)
				decode(tree, pair.second);
			else
				decode(tree.child(0), pair.second);
		}
		else if constexpr (is_pure_value<U>::value)
		{
			decode(tree, pair.second);
			if constexpr (is_pure_container<T>::value)
				decode(tree, pair.first);
			else
				decode(tree.child(0), pair.first);
		}
		else
		{
			decode(tree.child(0), pair.first);
			decode(tree.child(1), pair.second);
		}
	}

	template <typename MutableTree, typename T, typename U>
	void encode(const std::pair<T, U> &pair, MutableTree &tree)
	{
		if constexpr (is_pure_value<T>::value)
		{
			encode(pair.first, tree);
			if constexpr (is_pure_container<U>::value)
			{
				encode(pair.second, tree);
			}
			else
			{
				tree.child_count(1);
				encode(pair.second, tree.child(0));
			}
		}
		else if constexpr (is_pure_value<U>::value)
		{
			encode(pair.second, tree);
			if constexpr (is_pure_container<T>::value)
			{
				encode(pair.first, tree);
			}
			else
			{
				tree.child_count(1);
				encode(pair.first, tree.child(0));
			}
		}
		else
		{
			tree.child_count(2);
			encode(pair.first, tree.child(0));
			encode(pair.second, tree.child(1));
		}
	}
}

#endif
