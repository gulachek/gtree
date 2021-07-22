#ifndef GULACHEK_GTREE_ENCODING_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_ENCODING_HPP

#include "gulachek/gtree/error.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include <type_traits>
#include <utility>

namespace gulachek::gtree
{
	template <typename T>
	struct enable { using type = void; };

	template <typename T>
	using enable_t = typename enable<T>::type;

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

	template <typename T>
	struct encoding_t : encoding<std::decay_t<T>> {};

	// Can the type be decoded? Not intended for specialization.
	template <typename T>
	struct is_sertreealizable : std::conjunction<
										 std::is_default_constructible<T>,
										 std::is_move_constructible<T>,
										 std::is_move_assignable<T>
															> {};

	template <bool Anything>
	constexpr bool constexpr_bool = Anything;

	// can I enforce constexpr? these
	template <typename T>
	concept Encoding =
	requires(tree tr, mutable_tree mtr, typename T::type val)
	{
		{ constexpr_bool<T::uses_value> };
		{ constexpr_bool<T::uses_children> };
		{ T::encode(val, mtr) } -> std::same_as<error>;
		{ T::encode(std::move(val), mtr) } -> std::same_as<error>;
		{ T::decode(tr, val) } -> std::same_as<error>;
		{ T::decode(std::move(tr), val) } -> std::same_as<error>;
	};

	template <typename T>
	concept Sertreealizable_ =
	std::is_default_constructible_v<T> &&
	std::is_move_constructible_v<T> &&
	std::is_move_assignable_v<T> &&
	Encoding<encoding_t<T>>;

	template <typename T>
	concept Sertreealizable = Sertreealizable_<std::decay_t<T>>;

	// Does the type's encoding ever have a value?
	template <typename T>
	struct uses_value
	{
		static constexpr bool value = encoding_t<T>::uses_value;
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
		static constexpr bool value = encoding_t<T>::uses_children;
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

	template <Sertreealizable T, MutableTree Tree>
	error encode(const T &val, Tree &tr)
	{
		return encoding_t<T>::encode(val, tr);
	}

	template <Sertreealizable T, MutableTree Tree>
					 
	error encode(T &&val, Tree &tr) requires std::is_rvalue_reference_v<T>
	{
		return encoding_t<T>::encode(std::move(val), tr);
	}

	template <Tree Tr, Sertreealizable T>
	error decode(const Tr &tr, T &val)
	{
		return encoding_t<T>::decode(tr, val);
	}

	template <Tree Tr, Sertreealizable T>
	error decode(Tr &&tr, T &val) requires std::is_rvalue_reference_v<T>
	{
		return encoding_t<T>::decode(std::move(tr), val);
	}
}

#endif
