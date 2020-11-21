#ifndef GULACHEK_GTREE_ENCODING_EMPTY_HPP
#define GULACHEK_GTREE_ENCODING_EMPTY_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	// Annotate your empty class with a type member
	// This is more intrusive than it seems to enable for all
	// std::is_empty, especially for contrived tests
	template <typename T, typename Enable = void>
	struct empty_encoding {};

	template <typename T>
	struct __2vs
	{ using type = void*; };

	template <typename T>
	struct uses_value<
		T,
		typename __2vs<typename empty_encoding<T>::type>::type
	> : std::false_type {};

	template <typename T>
	struct uses_children<
		T,
		typename __2vs<typename empty_encoding<T>::type>::type
	> : std::false_type {};

	template <
		typename T,
		typename MutableTree,
		typename empty_encoding<T>::type* = nullptr
		>
	void encode(const T &val, MutableTree &tree)
	{
		tree = MutableTree{};
	}

	template <
		typename Tree,
		typename T,
		typename empty_encoding<T>::type* = nullptr
		>
	void decode(const Tree &tr, T &val) { }
}

#endif
