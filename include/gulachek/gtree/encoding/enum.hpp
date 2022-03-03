#ifndef GULACHEK_GTREE_ENCODING_ENUM_HPP
#define GULACHEK_GTREE_ENCODING_ENUM_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
		requires std::is_enum_v<T>
	struct decoding<T>
	{
		T *p;

		cause decode(treeder &r)
		{
			using underlying_t = std::underlying_type_t<T>;
			decoding<underlying_t> dec{(underlying_t*)p};
			return dec.decode(r);
		}
	};

	template <typename T>
		requires std::is_enum_v<T>
	struct encoding<T>
	{
		const T &val;

		cause encode(tree_writer &w)
		{
			using underlying_t = std::underlying_type_t<T>;
			encoding<underlying_t> enc{(const underlying_t&)val};
			return enc.encode(w);
		}
	};
	/*
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
	*/
}

#endif
