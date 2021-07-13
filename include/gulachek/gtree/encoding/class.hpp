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

	// Specify an encoding type that your class can convert
	// to/from
	template <typename MutableTree, typename T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	error encode(T &&val, MutableTree &tree)
	{
		typename T::gtree_encoding temp;

		if (auto err = val.gtree_encode(temp))
			return err;

		return encode(std::move(temp), tree);
	}

	template <typename MutableTree, typename T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	error encode(T &&val, MutableTree &tree)
	{
		return val.gtree_encode(tree);
	}

	template <typename Tree, typename T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	error decode(Tree &&tree, T &val)
	{
		typename T::gtree_encoding temp;

		if (auto err = decode(std::forward<Tree>(tree), temp))
			return err;

		return val.gtree_decode(std::move(temp));
	}

	template <typename Tree, typename T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	error decode(Tree &&tree, T &val)
	{
		return val.gtree_decode(std::forward<Tree>(tree));
	}

	template <typename T>
	struct uses_value<
		T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void>
			> :
		uses_value<typename T::gtree_encoding> {};

	template <typename T>
	struct uses_value<
		T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void>
			>
	{
		static constexpr bool value =
			boost::mpl::has_key<__manual_uses_value,
			typename T::gtree_encoding
		>::value;
	};

	template <typename T>
	struct uses_children<
		T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void>
			> :
		uses_children<typename T::gtree_encoding> {};

	template <typename T>
	struct uses_children<
		T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void>
			>
	{
		static constexpr bool value =
			boost::mpl::has_key<__manual_uses_children,
			typename T::gtree_encoding
		>::value;
	};
}

#endif
