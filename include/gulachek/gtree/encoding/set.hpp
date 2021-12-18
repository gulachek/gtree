#ifndef GULACHEK_GTREE_ENDODING_SET_HPP
#define GULACHEK_GTREE_ENDODING_SET_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <set>

namespace gulachek::gtree
{
	template <typename T>
	struct encoding<std::set<T>>
	{
		using type = std::set<T>;

		template <typename Set, typename MutableTree>
		static error encode(Set &&set, MutableTree &tree)
		{
			tree.child_count(set.size());

			std::size_t i = 0;
			for (auto &&elem : std::forward<Set>(set))
			{
				if (auto err = gtree::encode(elem, tree.child(i++)))
					return err;
			}

			return {};
		}

		template <typename Tree>
		static error decode(Tree &&tree, type &set)
		{
			set.clear();

			for (std::size_t i = 0; i < tree.child_count(); i++)
			{
				T elem;
				if (auto err = gtree::decode(std::forward<Tree>(tree).child(i), elem))
					return err;

				set.emplace(std::move(elem));
			}

			return {};
		}
	};
}

#endif
