#ifndef GULACHEK_GTREE_ENCODING_VECTOR_HPP
#define GULACHEK_GTREE_ENCODING_VECTOR_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <vector>

namespace gulachek::gtree
{
	template <typename T, typename Allocator>
	struct encoding<std::vector<T, Allocator>>
	{
		using type = std::vector<T, Allocator>;

		// Vectors parse children sequentially
		template <typename Vec, typename MutableTree>
		static error encode(
				Vec &&val,
				MutableTree &tree
				)
		{
			tree.child_count(val.size());

			for (std::size_t i = 0; i < val.size(); i++)
			{
				if (auto err = gtree::encode(at(std::forward<Vec>(val), i), tree.child(i)))
					return err;
			}

			return {};
		}

		template <typename Tree>
		static error decode(
				Tree &&tree,
				type &val
				)
		{
			val.clear();
			val.reserve(tree.child_count());

			for (std::size_t i = 0; i < tree.child_count(); i++)
			{
				T elem;
				if (auto err = gtree::decode(std::forward<Tree>(tree).child(i), elem))
					return err;

				val.emplace_back(std::move(elem));
			}

			return {};
		}

		private:
			static auto at(type &&vec, std::size_t i)
			{
				return std::move(vec[i]);
			}

			static auto at(const type &vec, std::size_t i)
			{
				return vec[i];
			}
	};
}

#endif
