#ifndef GULACHEK_GTREE_ENCODING_TUPLE_HPP
#define GULACHEK_GTREE_ENCODING_TUPLE_HPP

#include <tuple>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename ...Ts>
	struct uses_value<std::tuple<Ts...>> : std::false_type {};

	template <std::size_t n, typename Tree, typename Tuple,
		std::enable_if_t<n == std::tuple_size_v<Tuple>, int> = 0
			>
	void __encode(const Tuple &val, Tree &tree)
	{}

	template <std::size_t n, typename Tree, typename Tuple,
		std::enable_if_t<n < std::tuple_size_v<Tuple>, int> = 0
			>
	void __encode(const Tuple &val, Tree &tree)
	{
		encode(std::get<n>(val), tree.child(n));
		__encode<n+1>(val, tree);
	}

	template <typename MutableTree, typename ...Ts>
	void encode(const std::tuple<Ts...> &val, MutableTree &tree)
	{
		tree.child_count(sizeof...(Ts));
		__encode<0>(val, tree);
	}

	template <std::size_t n, typename Tree, typename Tuple,
		std::enable_if_t<n == std::tuple_size_v<Tuple>, int> = 0
			>
	void __decode(const Tree &tree, Tuple &val)
	{}

	template <std::size_t n, typename Tree, typename Tuple,
		std::enable_if_t<n < std::tuple_size_v<Tuple>, int> = 0
			>
	void __decode(const Tree &tree, Tuple &val)
	{
		decode(tree.child(n), std::get<n>(val));
		__decode<n+1>(tree, val);
	}

	template <typename Tree, typename ...Ts>
	void decode(const Tree &tree, std::tuple<Ts...> &val)
	{
		__decode<0>(tree, val);
	}
}

#endif
