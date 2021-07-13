#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

#include "gulachek/gtree/error.hpp"

namespace gulachek::gtree
{
	template <typename T>
	struct enable { using type = void; };

	template <typename T>
	using enable_t = typename enable<T>::type;

	// Does the type's encoding ever have a value?
	template <typename T, typename Enable = void>
	struct uses_value {};

	template <typename T>
	struct is_pure_container
	{
		static constexpr bool value = !uses_value<T>::value;
	};

	// Does the type's encoding ever have children?
	template <typename T, typename Enable = void>
	struct uses_children {};

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
	// Caller is responsible for making copy if value must be constant
	// template <typename MutableTree, typename T>
	// error encode(T &&val, MutableTree &tree);

	// Decode object from a tree
	// Caller is responsible for making copy if value must be constant
	// template <typename Tree, typename T>
	// error decode(Tree &&t, T &val);
	
	template <typename Tree, typename T,
		std::enable_if_t<
			!is_sertreealizable<T>::value
			, int> = 0
			>
	error decode(Tree &&, T &t)
	{
		static_assert(
				is_sertreealizable<T>::value
				, "Type must support decoding"
				);

		return "Type must support decoding";
	}

	template <typename Tree, typename T>
	error encode(const T &val, Tree &tr)
	{
		T cpy{val};
		return encode(std::move(cpy), tr);
	}
}

#endif
