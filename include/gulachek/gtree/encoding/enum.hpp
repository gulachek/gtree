#ifndef GULACHEK_GTREE_ENCODING_ENUM_HPP
#define GULACHEK_GTREE_ENCODING_ENUM_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct encoding<T, std::enable_if_t<std::is_enum_v<T>, void>>
	{
		using type = T;
		using underlying_t = std::underlying_type_t<type>;

		template <
			typename U,
			typename Tree
				>
		static error encode(
				U &&val,
				Tree &tree
				)
		{
			auto impl = static_cast<underlying_t>(val);
			return gtree::encode(impl, tree);
		}

		template <typename Tree>
		static error decode(
				Tree &&tree,
				type &val
				)
		{
			underlying_t impl;
			if (auto err = gtree::decode(std::forward<Tree>(tree), impl))
				return err;

			val = static_cast<T>(impl);
			return {};
		}
	};
}

#endif
