#ifndef GULACHEK_GTREE_ENCODING_CLASS_HPP
#define GULACHEK_GTREE_ENCODING_CLASS_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	struct manual_encoding {};

	template <typename T>
	struct encoding<T,
		std::enable_if_t<!std::is_same<manual_encoding, typename T::gtree_encoding>::value, void>>
	{
		using type = T;
		using src_t = typename type::gtree_encoding;

		template <typename MutableTree, typename ForwardRef>
		static error encode(ForwardRef &&val, MutableTree &tree)
		{
			src_t temp;

			if (auto err = std::forward<ForwardRef>(val).gtree_encode(temp))
				return err;

			return gtree::encode(std::move(temp), tree);
		}

		template <typename Tree>
		static error decode(Tree &&tree, type &val)
		{
			src_t temp;

			if (auto err = gtree::decode(std::forward<Tree>(tree), temp))
				return err;

			return val.gtree_decode(std::move(temp));
		}
	};

	template <typename T>
	struct encoding<T,
		std::enable_if_t<std::is_same<manual_encoding, typename T::gtree_encoding>::value, void>>
	{
		using type = T;

		template <typename MutableTree, typename ForwardRef>
		static error encode(ForwardRef &&val, MutableTree &tree)
		{
			return std::forward<ForwardRef>(val).gtree_encode(tree);
		}

		template <typename Tree>
		static error decode(Tree &&tree, type &val)
		{
			return val.gtree_decode(std::forward<Tree>(tree));
		}
	};
}

#endif
