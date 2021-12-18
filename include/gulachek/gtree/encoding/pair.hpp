#ifndef GULACHEK_GTREE_ENCODING_PAIR_HPP
#define GULACHEK_GTREE_ENCODING_PAIR_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	template <typename K, typename V>
	struct encoding<std::pair<K,V>>
	{
		using type = std::pair<K,V>;

		template <typename Tree>
		static error decode(Tree &&tree, type &pair)
		{
			if (tree.child_count() < 2)
				return {"expected 2 children for pair"};

			if (auto err =
					gtree::decode(std::forward<Tree>(tree).child(0),
						pair.first
						)
				 )
			{
				return err;
			}

			return gtree::decode(std::forward<Tree>(tree).child(1),
					pair.second);
		}

		template <typename Pair, typename MutableTree>
		static error encode(Pair &&pair, MutableTree &tree)
		{
			tree.child_count(2);
			if (auto err = gtree::encode(std::forward<Pair>(pair).first,
						tree.child(0)))
			{
				return err;
			}

			return gtree::encode(std::forward<Pair>(pair).second,
					tree.child(1));
		}
	};
}

#endif
