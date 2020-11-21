#ifndef GULACHEK_GTREE_ENCODING_MAP_HPP
#define GULACHEK_GTREE_ENCODING_MAP_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/pair.hpp"

#include <map>

namespace gulachek::gtree
{
	template <typename K, typename V, typename C, typename A>
	struct uses_value<std::map<K,V,C,A>> : std::false_type {};

	template <typename K, typename V, typename C, typename A>
	struct uses_children<std::map<K,V,C,A>> : std::true_type {};

	template <typename MutableTree, typename K, typename V, typename C, typename A>
	void encode(const std::map<K,V,C,A> &val, MutableTree &tree)
	{
		tree.child_count(val.size());

		std::size_t i = 0;
		for (const std::pair<K,V> &kv : val)
			encode(kv, tree.child(i++));
	}

	template <typename Tree, typename K, typename V, typename C, typename A>
	void decode(const Tree &tree, std::map<K, V, C, A> &val)
	{
		val.clear();

		for (std::size_t i = 0; i < tree.child_count(); i++)
		{
			std::pair<K, V> kv;
			decode(tree.child(i), kv);
			val.emplace(std::move(kv));
		}
	}
}

#endif
