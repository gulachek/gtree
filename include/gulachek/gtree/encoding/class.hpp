#ifndef GULACHEK_GTREE_ENCODING_CLASS_HPP
#define GULACHEK_GTREE_ENCODING_CLASS_HPP

#include <boost/mpl/set.hpp>
#include "gulachek/gtree/encoding/encoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	// Use this to encode your class/struct from raw trees
	struct value_encoding {};
	struct container_encoding {};
	struct hybrid_encoding {};

	using __manual_encodings = boost::mpl::set<
		value_encoding,
		container_encoding,
		hybrid_encoding
		>;

	using __manual_uses_value = boost::mpl::set<
		value_encoding,
		hybrid_encoding
		>;

	using __manual_uses_children = boost::mpl::set<
		container_encoding,
		hybrid_encoding
		>;

	template <typename T>
	struct encoding<T,
		std::enable_if_t<!boost::mpl::has_key<__manual_encodings, typename T::gtree_encoding>::value, void>>
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

		static constexpr bool uses_value = gtree::uses_value<src_t>::value;
		static constexpr bool uses_children = gtree::uses_children<src_t>::value;
	};

	template <typename T>
	struct encoding<T,
		std::enable_if_t<boost::mpl::has_key<__manual_encodings, typename T::gtree_encoding>::value, void>>
	{
		using type = T;
		using archetype = typename type::gtree_encoding;

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

		static constexpr bool uses_value = boost::mpl::has_key<__manual_uses_value, archetype>::value;
		static constexpr bool uses_children = boost::mpl::has_key<__manual_uses_children, archetype>::value;
	};
}

#endif
