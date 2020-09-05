#ifndef GULACHEK_GTREE_ENCODING_MAP_HPP
#define GULACHEK_GTREE_ENCODING_MAP_HPP

#include <map>

namespace gulachek::gtree
{
	template <typename K, typename V, typename C, typename A>
	struct uses_value<std::map<K,V,C,A>> : std::false_type {};

	template <typename MutableTree, typename K, typename V>
	void __encode_kv(const K &k, const V &v, MutableTree &tree)
	{
		if constexpr (
				(uses_children<K>::value || uses_value<V>::value) &&
				(uses_children<V>::value || uses_value<K>::value)
				)
		{
			// { K V }
			if constexpr (uses_children<K>::value)
			{
				tree.child_count(2);
				encode(k, tree.child(0));
				encode(v, tree.child(1));
			}
			// { K | V }
			else
			{
				tree.child_count(1);
				encode(k, tree);
				encode(v, tree.child(0));
			}
		}
		// K = V (same node)
		else
		{
			encode(k, tree);
			encode(v, tree);
		}
	}


	template <typename MutableTree, typename K, typename V, typename C, typename A>
	void encode(const std::map<K,V,C,A> &val, MutableTree &tree)
	{
		tree.child_count(val.size());

		std::size_t i = 0;
		for (const auto &kv : val)
			__encode_kv<MutableTree, K, V>(
					kv.first, kv.second, tree.child(i++));
	}

	template <typename Tree, typename K, typename V>
	void __decode_kv(const Tree &tree, K &k, V &v)
	{
		if constexpr (
				(uses_children<K>::value || uses_value<V>::value) &&
				(uses_children<V>::value || uses_value<K>::value)
				)
		{
			// { K V }
			if constexpr (uses_children<K>::value)
			{
				decode(tree.child(0), k);
				decode(tree.child(1), v);
			}
			// { K | V }
			else
			{
				decode(tree, k);
				decode(tree.child(0), v);
			}
		}
		// K = V (same node)
		else
		{
			decode(tree, k);
			decode(tree, v);
		}
	}

	template <typename Tree, typename K, typename V, typename C, typename A>
	void decode(const Tree &tree, std::map<K, V, C, A> &val)
	{
		val.clear();

		for (std::size_t i = 0; i < tree.child_count(); i++)
		{
			K k; V v;
			__decode_kv<Tree, K, V>(tree.child(i), k, v);
			val.emplace(std::move(k), std::move(v));
		}
	}
}

#endif
