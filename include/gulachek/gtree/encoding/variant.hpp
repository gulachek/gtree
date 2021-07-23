#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"

#include <variant>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename Variant>
	struct variant_encoding
	{};

	template <std::size_t I, typename Head, typename ...Tail>
	struct meta_cons_
	{
		static constexpr std::size_t index = I;
		static constexpr bool has_next = true;
		using type = Head;

		using next = meta_cons_<I+1, Tail...>;
	};

	template <std::size_t I, typename End>
	struct meta_cons_<I, End>
	{
		static constexpr std::size_t index = I;
		static constexpr bool has_next = false;
		using type = End;
	};

	template <typename ...Ts>
	struct meta_cons : meta_cons_<0, Ts...> {};

	// Specialization for std::variant
	template <typename ...Ts>
	struct variant_encoding<std::variant<Ts...>>
	{
		using types = meta_cons<Ts...>;

		template <typename Var>
		static std::size_t index(Var &&v)
		{ return std::forward<Var>(v).index(); }

		template <typename T, typename Var>
		static auto get(Var &&v)
		{ return std::get<T>(std::forward<Var>(v)); }
	};

	template <typename Variant>
	struct encoding<Variant, enable_t<typename variant_encoding<Variant>::types>>
	{
		using type = Variant;
		using var_encoding = variant_encoding<Variant>;
		using types = typename var_encoding::types;

		template <typename Types, typename Tree, typename V>
		static error encode_(V &&var, Tree &tree, std::size_t i)
		{
			using type = typename Types::type;

			if (i == Types::index)
			{
				auto &&ref = var_encoding::
					template get<type>(std::forward<V>(var));

				if constexpr (gtree::uses_value<type>::value)
				{
					tree.child_count(1);
					return gtree::encode(std::forward<type>(ref), tree.child(0));
				}
				else
				{
					return gtree::encode(std::forward<type>(ref), tree);
				}
			}

			if constexpr (Types::has_next)
			{
				return encode_<typename Types::next>(std::forward<V>(var), tree, i);
			}

			return error{"Variant index out of range"};
		}

		// Variant has type index as value and data as first child
		template <
			typename Tree,
			typename V
				 >
		static error encode(
				V &&val,
				Tree &tree
				)
		{
			// can I use a const lvalue to pass here?
			auto index = var_encoding::index(std::forward<V>(val));

			// assume integers can't fail encoding
			gtree::encode(index, tree);
			return encode_<types>(std::forward<V>(val), tree, index);
		}

		template <typename Types, typename Tree>
		static error decode_(Tree &&tree, type &val, std::size_t i)
		{
			using type = typename Types::type;

			if (i == Types::index)
			{
				type elem;

				// No need for indirection if no value is in the way
				error err;
				if constexpr (gtree::uses_value<type>::value)
					err = gtree::decode(std::forward<Tree>(tree).child(0), elem);
				else
					err = gtree::decode(std::forward<Tree>(tree), elem);

				if (err) return err;
				val = std::move(elem);
				return {};
			}

			if constexpr (Types::has_next)
			{
				return decode_<typename Types::next>
					(std::forward<Tree>(tree), val, i);
			}

			return error{"Variant index out of range"};
		}

		template <
			typename Tree
			>
		static error decode(
				Tree &&tree,
				type &val
				)
		{
			std::size_t index;

			if (auto err = gtree::decode(std::forward<Tree>(tree), index))
				return err;

			return decode_<types>(std::forward<Tree>(tree), val, index);
		}

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = true;
	};
}

#endif
