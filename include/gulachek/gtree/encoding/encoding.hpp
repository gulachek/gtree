#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

namespace gulachek::gtree
{
	// Does the type's encoding ever have a value? Opt out;
	template <typename T, typename Extend = void*>
	struct uses_value : std::true_type {};

	// Does the type's encoding ever have children? Opt out.
	template <typename T, typename Extend = void*>
	struct uses_children : std::true_type {};

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

	// Specify an encoding type that your class can convert
	// to/from
	template <typename MutableTree, typename T,
		typename T::gtree_encoding* = nullptr
			>
	void encode(const T &val, MutableTree &tree)
	{
		using Encoding = typename T::gtree_encoding;
		const Encoding &temp = T::gtree_encode(val);
		encode(temp, tree);
	}

	template <typename Tree, typename T,
		typename T::gtree_encoding* = nullptr
			>
	void decode(const Tree &tree, T &val)
	{
		using Encoding = typename T::gtree_encoding;
		Encoding temp;
		decode(tree, temp);
		val = T::gtree_decode(std::move(temp));
	}
}

#endif
