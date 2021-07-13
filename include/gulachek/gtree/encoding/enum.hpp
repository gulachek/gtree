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
	error encode(
			T &&val,
			Tree &tree
			)
	{
		using under_type = std::underlying_type_t<T>;
		auto &&impl = static_cast<under_type>(std::forward<T>(val));
		return encode(std::forward<under_type>(impl), tree);
	}

	template <
		typename Tree,
		typename T,
		std::enable_if_t<std::is_enum_v<T>, void*> = nullptr
			>
	error decode(
			Tree &&tree,
			T &val
			)
	{
		std::underlying_type_t<T> impl;
		if (auto err = decode(std::forward<Tree>(tree), impl))
			return err;

		val = static_cast<T>(impl);
		return {};
	}
}

#endif
