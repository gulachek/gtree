#ifndef GULACHEK_GTREE_ENCODING_PAIR_HPP
#define GULACHEK_GTREE_ENCODING_PAIR_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	template <typename K, typename V>
	struct encoding<std::pair<K,V>>
	{
		using type = std::pair<K,V>;

		static constexpr bool uses_children = true;
		static constexpr bool uses_value = 
				is_pure_value<K>::value || is_pure_value<V>::value;

		template <typename Tree>
		static error decode(Tree &&tree, type &pair)
		{
			if constexpr (is_pure_value<K>::value)
			{
				if (auto err = gtree::decode(std::forward<Tree>(tree), pair.first))
					return err;

				if constexpr (is_pure_container<V>::value)
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree), pair.second))
						return err;
				}
				else
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree).child(0), pair.second))
						return err;
				}
			}
			else if constexpr (is_pure_value<V>::value)
			{
				if (auto err = gtree::decode(std::forward<Tree>(tree), pair.second))
					return err;

				if constexpr (is_pure_container<K>::value)
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree), pair.first))
						return err;
				}
				else
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree).child(0), pair.first))
						return err;
				}
			}
			else
			{
				if (auto err = gtree::decode(std::forward<Tree>(tree).child(0), pair.first))
					return err;

				if (auto err = gtree::decode(std::forward<Tree>(tree).child(1), pair.second))
					return err;
			}

			return {};
		}

		template <typename Pair, typename MutableTree>
		static error encode(Pair &&pair, MutableTree &tree)
		{
			if constexpr (is_pure_value<K>::value)
			{
				if (auto err = gtree::encode(std::forward<Pair>(pair).first, tree))
					return err;

				if constexpr (is_pure_container<V>::value)
				{
					if (auto err = gtree::encode(std::forward<Pair>(pair).second, tree))
						return err;
				}
				else
				{
					tree.child_count(1);
					if (auto err = gtree::encode(std::forward<Pair>(pair).second, tree.child(0)))
						return err;
				}
			}
			else if constexpr (is_pure_value<V>::value)
			{
				if (auto err = gtree::encode(std::forward<Pair>(pair).second, tree))
					return err;

				if constexpr (is_pure_container<K>::value)
				{
					if (auto err = gtree::encode(std::forward<Pair>(pair).first, tree))
						return err;
				}
				else
				{
					tree.child_count(1);
					if (auto err = gtree::encode(std::forward<Pair>(pair).first, tree.child(0)))
						return err;
				}
			}
			else
			{
				tree.child_count(2);
				if (auto err = gtree::encode(std::forward<Pair>(pair).first, tree.child(0)))
					return err;

				if (auto err = gtree::encode(std::forward<Pair>(pair).second, tree.child(1)))
					return err;
			}

			return {};
		}
	};
}

#endif
