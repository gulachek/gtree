#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

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
	struct raw_encoding {};

	// Specify an encoding type that your class can convert
	// to/from
	template <typename MutableTree, typename T,
		std::enable_if_t<
			!std::is_same_v<typename T::gtree_encoding, raw_encoding>,
			void*> = nullptr
			>
	void encode(const T &val, MutableTree &tree)
	{
		typename T::gtree_encoding temp;
		val.gtree_encode(temp);
		encode(temp, tree);
	}

	template <typename Tree, typename T,
		std::enable_if_t<
			!std::is_same_v<typename T::gtree_encoding, raw_encoding>,
			void*> = nullptr
			>
	void decode(const Tree &tree, T &val)
	{
		typename T::gtree_encoding temp;
		decode(tree, temp);
		val.gtree_decode(temp);
	}

	template <typename T>
	struct __subst_void_star
	{
		using type = void*;
	};

	template <typename T>
	struct uses_value<
		T,
		typename __subst_void_star<typename T::gtree_encoding>::type
			> :
		uses_value<typename T::gtree_encoding> {};

	template <typename T>
	struct uses_children<
		T,
		typename __subst_void_star<typename T::gtree_encoding>::type
			> :
		uses_children<typename T::gtree_encoding> {};
}

#endif
