#ifndef GULACHEK_GTREE_ENCODING_TREE_HPP
#define GULACHEK_GTREE_ENCODING_TREE_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<
		T,
		std::enable_if_t<is_tree<T>::value, void>
		> : std::true_type {};

	template <typename T>
	struct uses_children<
		T,
		std::enable_if_t<is_tree<T>::value, void>
		> : std::true_type {};

	template <typename Tree, typename MutableTree>
	void copy_tree(const Tree &src, MutableTree &dest)
	{
		auto val = src.value();
		dest.value(val.data(), val.data() + val.size());

		auto n = src.child_count();
		dest.child_count(n);

		for (std::size_t i = 0; i < n; i++)
			copy_tree(src.child(i), dest.child(i));
	}

	// optimize this later to check to see if trees are
	// same type
	template <
		typename T,
		typename MutableTree,
		std::enable_if_t<is_tree<T>::value, int> = 0
			>
	void encode(
			const T &src,
			MutableTree &dest
			)
	{
		copy_tree(src, dest);
	}

	template <
		typename Tree,
	 	typename T,
		std::enable_if_t<is_mutable_tree<T>::value, int> = 0
			>
	void decode(
			const Tree &src,
			T &dest
			)
	{
		copy_tree(src, dest);
	}
}

#endif
