#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

#include <boost/mpl/set.hpp>

namespace gulachek::gtree
{
	// Does the type's encoding ever have a value? Opt out;
	template <typename T, typename Extend = void*>
	struct uses_value : std::true_type {};

	template <typename T>
	struct is_pure_container
	{
		static constexpr bool value = !uses_value<T>::value;
	};

	// Does the type's encoding ever have children? Opt out.
	template <typename T, typename Extend = void*>
	struct uses_children : std::true_type {};

	template <typename T>
	struct is_pure_value
	{
		static constexpr bool value = !uses_children<T>::value;
	};

	// Can the type be decoded? Not intended for specialization.
	template <typename T>
	struct is_sertreealizable : std::conjunction<
										 std::is_default_constructible<T>,
										 std::is_move_constructible<T>,
										 std::is_move_assignable<T>
															> {};

	// Encode object into a tree
	// template <typename MutableTree, typename T>
	// void encode(const T &val, MutableTree &tree);

	// Decode object from a tree
	// template <typename Tree, typename T>
	// void decode(const Tree &t, T &val);
	
	template <typename Tree, typename T,
		std::enable_if_t<
			!is_sertreealizable<T>::value
			, int> = 0
			>
	void decode(const Tree &, T &t)
	{
		static_assert(
				is_sertreealizable<T>::value
				, "Type must support decoding"
				);
	}

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
	void encode(const T &val, MutableTree &tree)
	{
		typename T::gtree_encoding temp;
		val.gtree_encode(temp);
		encode(temp, tree);
	}

	template <typename MutableTree, typename T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	void encode(const T &val, MutableTree &tree)
	{
		val.gtree_encode(tree);
	}

	template <typename Tree, typename T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	void decode(const Tree &tree, T &val)
	{
		typename T::gtree_encoding temp;
		decode(tree, temp);
		val.gtree_decode(temp);
	}

	template <typename Tree, typename T,
		std::enable_if_t<
			boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*> = nullptr
			>
	void decode(const Tree &tree, T &val)
	{
		val.gtree_decode(tree);
	}

	template <typename T>
	struct __subst_void_star
	{
		using type = void*;
	};

	template <typename T>
	struct uses_value<
		T,
		std::enable_if_t<
			!boost::mpl::has_key<
				__manual_encodings,
				typename T::gtree_encoding
			>::value,
			void*>
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
			void*>
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
			void*>
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
			void*>
			>
	{
		static constexpr bool value =
			boost::mpl::has_key<__manual_uses_children,
			typename T::gtree_encoding
		>::value;
	};
}

#endif
