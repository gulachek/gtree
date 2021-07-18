#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

#include "gulachek/gtree/error.hpp"
#include <utility>

namespace gulachek::gtree
{
	template <typename T>
	struct enable { using type = void; };

	template <typename T>
	using enable_t = typename enable<T>::type;

	// Can the type be decoded? Not intended for specialization.
	template <typename T>
	struct is_sertreealizable : std::conjunction<
										 std::is_default_constructible<T>,
										 std::is_move_constructible<T>,
										 std::is_move_assignable<T>
															> {};

	// encoding concept
	template <typename T, typename Enable = void>
	struct encoding
	{
		// using type = T;

		// static constexpr bool uses_value;
		// static constexpr bool uses_children;

		// template <typename T, typename MutableTree>
		// static error encode(T &&val, MutableTree &tr);

		// template <typename Tree, typename T>
		// static error decode(Tree &&tr, T &val);
	};

	// Does the type's encoding ever have a value?
	template <typename T>
	struct uses_value
	{
		static constexpr bool value = encoding<T>::uses_value;
	};

	template <typename T>
	struct is_pure_container
	{
		static constexpr bool value = !uses_value<T>::value;
	};

	// Does the type's encoding ever have children?
	template <typename T>
	struct uses_children
	{
		static constexpr bool value = encoding<T>::uses_children;
	};

	template <typename T>
	struct is_pure_value
	{
		static constexpr bool value = !uses_children<T>::value;
	};

	// Encode object into a tree
	// Caller is responsible for making copy if value must be constant
	// template <typename T, typename MutableTree>
	// error encode(T &&val, MutableTree &tree);

	// Decode object from a tree
	// Caller is responsible for making copy if value must be constant
	// template <typename Tree, typename T>
	// error decode(Tree &&t, T &val);

	template <typename Tree, typename T>
	error encode(T &&val, Tree &tr)
	{
		return encoding<std::decay_t<T>>::encode(std::forward<T>(val), tr);
	}

	template <typename Tree, typename T>
	error decode(Tree &&tr, T &val)
	{
		return encoding<std::decay_t<T>>::decode(std::forward<Tree>(tr), val);
	}
}

#endif
