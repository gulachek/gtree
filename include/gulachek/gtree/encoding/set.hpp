#ifndef GULACHEK_GTREE_ENDODING_SET_HPP
#define GULACHEK_GTREE_ENDODING_SET_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <set>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<std::set<T>> : std::false_type {};

	template <typename MutableTree, typename T>
	void encode(const std::set<T> &set, MutableTree &tree)
	{
		tree.child_count(set.size());

		std::size_t i = 0;
		for (const T& elem : set)
			encode(elem, tree.child(i++));
	}

	template <typename T, typename Tree>
	void decode(const Tree &tree, std::set<T> &set)
	{
		set.clear();

		for (std::size_t i = 0; i < tree.child_count(); i++)
		{
			T elem;
			decode(tree.child(i), elem);
			set.emplace(std::move(elem));
		}
	}
}

#endif
