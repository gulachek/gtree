#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

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

		static std::size_t index(const std::variant<Ts...> &v)
		{ return v.index(); }

		template <typename T>
		static const T& get(const std::variant<Ts...> &v)
		{ return std::get<T>(v); }
	};

	template <
		std::size_t I,
		typename Sequence,
		typename Function,
		std::enable_if_t<
			I >= boost::mpl::size<Sequence>::value,
			void*> = nullptr
			>
	void __run_at_elem(std::size_t i, Function &&f)
	{
		// don't worry, be happy
	}

	template <
		std::size_t I,
		typename Sequence,
		typename Function,
		std::enable_if_t<
			I < boost::mpl::size<Sequence>::value,
			void*> = nullptr
			>
	void __run_at_elem(std::size_t i, Function &&f)
	{
		if (i == I)
		{
			boost::value_initialized<
				typename boost::mpl::at_c<Sequence, I>::type
				> v;

			std::invoke(
					std::forward<Function>(f),
					boost::get(v)
					);
		}
		else
		{
			__run_at_elem<I+1, Sequence, Function>(
					i,
					std::forward<Function>(f)
					);
		}
	}

	template <typename Sequence, typename Function>
	void __run_at_elem(std::size_t i, Function &&f)
	{
		__run_at_elem<0, Sequence, Function>
			(i, std::forward<Function>(f));
	}

	template <typename Variant, typename Tree>
	struct __encode_functor
	{
		const Variant &val;
		Tree &tr;

		template <typename T>
		void operator() (T)
		{
			auto &ref = variant_encoding<Variant>::
				template get<T>(val);

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
	};

	// Variant has type index as value and data as first child
	template <
		typename Tree,
		typename Variant,
		typename variant_encoding<Variant>::sequence* = nullptr
			 >
	void encode(
			const Variant &val,
			Tree &tree
			)
	{
		typedef typename variant_encoding<Variant>::sequence seq;
		auto index = variant_encoding<Variant>::index(val);

		encode(index, tree);

		__encode_functor<Variant, Tree> ftor{val, tree};
		__run_at_elem<seq>(index, ftor);
	}

	template <typename Variant, typename Tree>
	struct __decode_functor
	{
		const Tree &tr;
		Variant &val;

		template <typename T>
		void operator() (T)
		{
			T elem;

			// No need for indirection if no value is in the way
			if constexpr (uses_value<T>::value)
				decode(tr.child(0), elem);
			else
				decode(tr, elem);

			val = std::move(elem);
		}
	};

	template <
		typename Tree,
		typename Variant,
		typename variant_encoding<Variant>::sequence* = nullptr
		>
	void decode(
			const Tree &tree,
			Variant &val
			)
	{
		typedef typename variant_encoding<Variant>::sequence seq;
		std::size_t index;
		decode(tree, index);

		__decode_functor<Variant, Tree> ftor{tree, val};
		__run_at_elem<seq>(index, ftor);
	}
}

#endif
