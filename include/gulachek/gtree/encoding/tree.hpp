#ifndef GULACHEK_GTREE_ENCODING_TREE_HPP
#define GULACHEK_GTREE_ENCODING_TREE_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <typename U>
	struct encoding<U, std::enable_if_t<is_tree<U>::value, void>>
	{
		static constexpr bool uses_value = true;
		static constexpr bool uses_children = true;

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

		template <typename Tree, typename T>
		static error decode(
				Tree &&src,
				T &dest
				)
		{
			copy_tree(std::forward<Tree>(src), dest);
			return {};
		}

		private:
			template <typename Tree, typename MutableTree>
			static void copy_tree(Tree &&src, MutableTree &dest)
			{
				auto val = src.value();
				dest.value(val.data(), val.data() + val.size());

				auto n = src.child_count();
				dest.child_count(n);

				for (std::size_t i = 0; i < n; i++)
					copy_tree(src.child(i), dest.child(i));
			}
	};
}

#endif
