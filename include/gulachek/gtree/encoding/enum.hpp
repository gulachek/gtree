#ifndef GULACHEK_GTREE_ENCODING_ENUM_HPP
#define GULACHEK_GTREE_ENCODING_ENUM_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<
		T,
		std::enable_if_t<std::is_enum_v<T>, void>
		> : uses_value<std::underlying_type_t<T>> {};

	template <typename T>
	struct uses_children<
		T,
		std::enable_if_t<std::is_enum_v<T>, void>
		> : uses_children<std::underlying_type_t<T>> {};

	template <
		typename Tree,
		typename T,
		std::enable_if_t<std::is_enum_v<T>, void*> = nullptr
			>
	void encode(
			const T &val,
			Tree &tree
			)
	{
		auto impl = static_cast<std::underlying_type_t<T>>(val);

		encode(impl, tree);
	}

	template <
		typename Tree,
		typename T,
		std::enable_if_t<std::is_enum_v<T>, void*> = nullptr
			>
	void decode(
			const Tree &tree,
			T &val
			)
	{
		std::underlying_type_t<T> impl;
		decode(tree, impl);
		val = static_cast<T>(impl);
	}
}

#endif
