#ifndef GULACHEK_GTREE_ENCODING_TREE_HPP
#define GULACHEK_GTREE_ENCODING_TREE_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <Tree U>
	struct encoding<U>
	{
		using type = U;

		// optimize this later to check to see if trees are
		// same type
		template <typename T, typename MutableTree>
		static error encode(
				T &&src,
				MutableTree &dest
				)
		{
			copy_tree(std::forward<T>(src), dest);
			return {};
		}

		template <Tree Tr, typename T>
		static error decode(
				Tr &&src,
				T &dest
				)
		{
			copy_tree(std::forward<Tr>(src), dest);
			return {};
		}

		private:
			template <Tree Tr, typename MutableTree>
			static void copy_tree(Tr &&src, MutableTree &dest)
			{
				dest.value(src.value());

				auto n = src.child_count();
				dest.child_count(n);

				for (std::size_t i = 0; i < n; i++)
					copy_tree(src.child(i), dest.child(i));
			}
	};
}

#endif
