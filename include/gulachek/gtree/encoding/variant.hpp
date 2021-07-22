#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"

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

		template <typename Var>
		static std::size_t index(Var &&v)
		{ return std::forward<Var>(v).index(); }

		template <typename T, typename Var>
		static auto get(Var &&v)
		{ return std::get<T>(std::forward<Var>(v)); }
	};

	template <typename Variant>
	struct encoding<Variant, enable_t<typename variant_encoding<Variant>::sequence>>
	{
		using type = Variant;
		using var_encoding = variant_encoding<Variant>;

		template <
			std::size_t I,
			typename Sequence,
			typename Function,
			typename ForwardRef,
			std::enable_if_t<
				I >= boost::mpl::size<Sequence>::value,
				void*> = nullptr
				>
		static error __run_at_elem(std::size_t i, Function &&f, ForwardRef &&ref)
		{
			return "Variant index out of range";
		}

		template <
			std::size_t I,
			typename Sequence,
			typename Function,
			typename ForwardRef,
			std::enable_if_t<
				I < boost::mpl::size<Sequence>::value,
				void*> = nullptr
				>
		static error __run_at_elem(std::size_t i, Function &&f, ForwardRef &&ref)
		{
			if (i == I)
			{
				boost::value_initialized<
					typename boost::mpl::at_c<Sequence, I>::type
					> v;

				return std::invoke(
						std::forward<Function>(f),
						boost::get(v),
						std::forward<ForwardRef>(ref)
						);
			}
			else
			{
				return __run_at_elem<I+1, Sequence, Function, ForwardRef>(
						i,
						std::forward<Function>(f),
						std::forward<ForwardRef>(ref)
						);
			}
		}

		template <typename Sequence, typename Function, typename ForwardRef>
		static auto __run_at_elem(std::size_t i, Function &&f, ForwardRef &&ref)
		{
			return __run_at_elem<0, Sequence, Function, ForwardRef>
				(i, std::forward<Function>(f), std::forward<ForwardRef>(ref));
		}

		template <typename Tree>
		struct __encode_functor
		{
			Tree &tr;

			template <typename T, typename Var>
			error operator() (T, Var &&val)
			{
				auto &&ref = var_encoding::
					template get<T>(std::forward<Var>(val));

				if constexpr (gtree::uses_value<T>::value)
				{
					tr.child_count(1);
					return gtree::encode(std::forward<T>(ref), tr.child(0));
				}
				else
				{
					return gtree::encode(std::forward<T>(ref), tr);
				}
			}
		};

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
			typedef typename var_encoding::sequence seq;

			// can I use a const lvalue to pass here?
			auto index = var_encoding::index(std::forward<V>(val));

			// assume integers can't fail encoding
			gtree::encode(index, tree);

			__encode_functor<Tree> ftor{tree};
			return __run_at_elem<seq>(index, ftor, std::forward<V>(val));
		}

		struct __decode_functor
		{
			type &val;

			template <typename T, typename Tree>
			error operator() (T, Tree &&tr)
			{
				T elem;

				// No need for indirection if no value is in the way
				error err;
				if constexpr (gtree::uses_value<T>::value)
					err = gtree::decode(std::forward<Tree>(tr).child(0), elem);
				else
					err = gtree::decode(std::forward<Tree>(tr), elem);

				if (err) return err;
				val = std::move(elem);
				return {};
			}
		};

		template <
			typename Tree
			>
		static error decode(
				Tree &&tree,
				type &val
				)
		{
			typedef typename var_encoding::sequence seq;
			std::size_t index;
			if (auto err = gtree::decode(std::forward<Tree>(tree), index))
				return err;

			__decode_functor ftor{val};
			return __run_at_elem<seq>(index, ftor, std::forward<Tree>(tree));
		}

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = true;
	};
}

#endif
