#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

#include <variant>
#include <type_traits>

namespace gulachek::gtree
{
	template <
		std::size_t n,
		typename Tree,
		typename Variant,
		std::enable_if_t<n == std::variant_size_v<Variant>, int> = 0
						 >
	void __encode_variant(
			const Variant &val,
			Tree &tr
			)
	{
		// don't worry, be happy
	}

	template <
		std::size_t n,
		typename Tree,
		typename Variant,
		std::enable_if_t<n < std::variant_size_v<Variant>, int> = 0
						 >
	void __encode_variant(
			const Variant &val,
			Tree &tr
			)
	{
		if (n == val.index())
		{
			using T = typename std::variant_alternative<n, Variant>::type;
			auto &ref = std::get<T>(val);

			if constexpr (uses_value<T>::value)
			{
				tr.child_count(1);
				encode(ref, tr.child(0));
			}
			else
			{
				encode(ref, tr);
			}
		}

		__encode_variant<n+1>(val, tr);
	}

	// Variant has type index as value and data as first child
	template <typename Tree, typename... Types>
	void encode(
			const std::variant<Types...> &val,
			Tree &tr
			)
	{
		// we need to write the index and reserve a child
		encode(val.index(), tr);

		__encode_variant<0>(val, tr);
	}

	template <
		std::size_t n,
		typename Tree,
		typename Variant,
		std::enable_if_t<n == std::variant_size_v<Variant>, int> = 0
						 >
	void __decode_variant(
			const Tree &tr,
			Variant &val,
			std::size_t index
			)
	{
		// don't care if not following conventions
		// just return
	}


	// This could eventually be more efficient
	template <
		std::size_t n,
		typename Tree,
		typename Variant,
		std::enable_if_t<n < std::variant_size_v<Variant>, int> = 0
						 >
	void __decode_variant(
			const Tree &tr,
			Variant &val,
			std::size_t index
			)
	{
		if (index == n)
		{
			std::variant_alternative_t<n, Variant> elem;

			// No need for indirection if no value is in the way
			if constexpr (uses_value<decltype(elem)>::value)
				decode(tr.child(0), elem);
			else
				decode(tr, elem);

			val = std::move(elem);
		}

		__decode_variant<n+1, Tree, Variant>(tr, val, index);
	}

	template <typename Tree, typename... Types>
	void decode(
			const Tree &tr,
			std::variant<Types...> &val
			)
	{
		// read the index
		std::size_t index;
		decode(tr, index);

		__decode_variant<0>(tr, val, index);
	}

}

#endif
