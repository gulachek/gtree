#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/size.hpp>
#include <boost/utility/value_init.hpp>
#include <variant>
#include <type_traits>

namespace gulachek::gtree
{
	// Specialize this with:
	// 	MPL sequence of variant alternative types
	// 	typedef ... sequence;
	//
	// 	Runtime index of an instance of the variant type
	// 	std::size_t index(const Variant &);
	template <typename Variant>
	struct variant_encoding
	{};

	// Specialization for std::variant
	template <typename ...Ts>
	struct variant_encoding<std::variant<Ts...>>
	{
		typedef typename boost::mpl::vector<Ts...>::type sequence;

		static std::size_t index(std::variant<Ts...> &&v)
		{ return v.index(); }

		template <typename T>
		static T&& get(std::variant<Ts...> &&v)
		{ return std::get<T>(std::forward<std::variant<Ts...>>(v)); }
	};

	template <
		std::size_t I,
		typename Sequence,
		typename Function,
		std::enable_if_t<
			I >= boost::mpl::size<Sequence>::value,
			void*> = nullptr
			>
	error __run_at_elem(std::size_t i, Function &&f)
	{
		return "Variant index out of range";
	}

	template <
		std::size_t I,
		typename Sequence,
		typename Function,
		std::enable_if_t<
			I < boost::mpl::size<Sequence>::value,
			void*> = nullptr
			>
	error __run_at_elem(std::size_t i, Function &&f)
	{
		if (i == I)
		{
			boost::value_initialized<
				typename boost::mpl::at_c<Sequence, I>::type
				> v;

			return std::invoke(
					std::forward<Function>(f),
					boost::get(v)
					);
		}
		else
		{
			return __run_at_elem<I+1, Sequence, Function>(
					i,
					std::forward<Function>(f)
					);
		}
	}

	template <typename Sequence, typename Function>
	auto __run_at_elem(std::size_t i, Function &&f)
	{
		return __run_at_elem<0, Sequence, Function>
			(i, std::forward<Function>(f));
	}

	template <typename Variant, typename Tree>
	struct __encode_functor
	{
		Variant &&val;
		Tree &tr;

		template <typename T>
		error operator() (T)
		{
			auto &&ref = variant_encoding<Variant>::
				template get<T>(std::forward<Variant>(val));

			if constexpr (uses_value<T>::value)
			{
				tr.child_count(1);
				return encode(std::forward<T>(ref), tr.child(0));
			}
			else
			{
				return encode(std::forward<T>(ref), tr);
			}
		}
	};

	// Variant has type index as value and data as first child
	template <
		typename Tree,
		typename Variant,
		typename variant_encoding<Variant>::sequence* = nullptr
			 >
	error encode(
			Variant &&val,
			Tree &tree
			)
	{
		typedef typename variant_encoding<Variant>::sequence seq;

		// can I use a const lvalue to pass here?
		auto index = variant_encoding<Variant>::index(std::forward<Variant>(val));

		// assume integers can't fail encoding
		encode(index, tree);

		__encode_functor<Variant, Tree> ftor{std::forward<Variant>(val), tree};
		return __run_at_elem<seq>(index, ftor);
	}

	template <typename Variant, typename Tree>
	struct __decode_functor
	{
		Tree &&tr;
		Variant &val;

		template <typename T>
		error operator() (T)
		{
			T elem;

			// No need for indirection if no value is in the way
			error err;
			if constexpr (uses_value<T>::value)
				err = decode(std::move(tr.child(0)), elem);
			else
				err = decode(std::move(tr), elem);

			if (err) return err;
			val = std::move(elem);
			return {};
		}
	};

	template <
		typename Tree,
		typename Variant,
		typename variant_encoding<Variant>::sequence* = nullptr
		>
	error decode(
			Tree &&tree,
			Variant &val
			)
	{
		typedef typename variant_encoding<Variant>::sequence seq;
		std::size_t index;
		if (auto err = decode(tree, index))
			return err;

		__decode_functor<Variant, Tree> ftor{std::forward<Tree>(tree), val};
		return __run_at_elem<seq>(index, ftor);
	}

	template <typename T>
	struct uses_value<
		T,
		enable_t<typename variant_encoding<T>::sequence>
	> : std::true_type {};

	template <typename T>
	struct uses_children<
		T,
		enable_t<typename variant_encoding<T>::sequence>
	> : std::true_type {};
}

#endif
