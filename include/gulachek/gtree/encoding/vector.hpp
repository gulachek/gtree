#ifndef GULACHEK_GTREE_ENCODING_VECTOR_HPP
#define GULACHEK_GTREE_ENCODING_VECTOR_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <vector>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<std::vector<T>> : std::false_type {};

	// Vectors parse children sequentially
	template <typename MutableTree,
					 typename T, typename Allocator>
	void encode(
			const std::vector<T, Allocator> &val,
			MutableTree &tree
			)
	{
		tree.child_count(val.size());

		for (std::size_t i = 0; i < val.size(); i++)
			encode(val[i], tree.child(i));
	}

	template <typename Tree, typename T, typename Allocator>
	void decode(
			const Tree &tree,
			std::vector<T, Allocator> &val
			)
	{
		val.clear();
		val.reserve(tree.child_count());

		for (std::size_t i = 0; i < tree.child_count(); i++)
		{
			T elem;
			decode(tree.child(i), elem);
			val.emplace_back(std::move(elem));
		}
	}
}

#endif
