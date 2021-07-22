#ifndef GULACHEK_GTREE_ENCODING_MAP_HPP
#define GULACHEK_GTREE_ENCODING_MAP_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/pair.hpp"

#include <map>

namespace gulachek::gtree
{
	template <typename K, typename V, typename C, typename A>
	struct encoding<std::map<K,V,C,A>>
	{
		using type = std::map<K,V,C,A>;

		static constexpr bool uses_value = false;
		static constexpr bool uses_children = true;

		template <typename Map, MutableTree Tree>
		static error encode(Map &&val, Tree &tree)
		{
			tree.child_count(val.size());

			std::size_t i = 0;
			for (auto &&kv : std::forward<Map>(val))
			{
				if (auto err =
						gtree::encode(std::forward<std::pair<K,V>>(kv), tree.child(i++)))
					return err;
			}

			return {};
		}

		template <Tree Tr>
		static error decode(Tr &&tree, type &val)
		{
			val.clear();

			for (std::size_t i = 0; i < tree.child_count(); i++)
			{
				std::pair<K, V> kv;

				if (auto err = gtree::decode(std::forward<Tr>(tree).child(i), kv))
					return err;

				val.emplace(std::move(kv));
			}

			return {};
		}
	};
}

#endif
