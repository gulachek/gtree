#ifndef GULACHEK_GTREE_ENCODING_FUSION_HPP
#define GULACHEK_GTREE_ENCODING_FUSION_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <tuple>
#include <type_traits>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace gulachek::gtree
{
	template <typename T>
	struct __enable_if_seq : std::enable_if<
			boost::fusion::traits::is_sequence<T>::value, void> {};

	template <typename T>
	using __enable_if_seq_t =
		typename __enable_if_seq<T>::type;

	template <typename T>
	struct uses_value<
		T,
		__enable_if_seq_t<T>
		>
	{
		static constexpr bool _value()
		{
			using namespace boost::fusion::result_of;

			constexpr auto n = size<T>::value;

			if constexpr (n == 1)
			{
				using first = typename begin<T>::type;
				using t = typename value_of<first>::type;
				return uses_value<t>::value;
			}
			else if constexpr (n == 2)
			{
				using left_it = typename begin<T>::type;
				using left = typename value_of<left_it>::type;
				using right_it = typename next<left_it>::type;
				using right = typename value_of<right_it>::type;

				return is_pure_value<left>::value ||
					is_pure_value<right>::value;
			}
			
			return false;
		}

		public:
			static constexpr bool value = _value();
	};

	template <typename T>
	struct uses_children<
		T,
		__enable_if_seq_t<T>
		>
	{
		static constexpr bool _value()
		{
			using namespace boost::fusion::result_of;

			constexpr auto n = size<T>::value;

			if constexpr (n == 1)
			{
				using first = typename begin<T>::type;
				using t = typename value_of<first>::type;
				return uses_children<t>::value;
			}

			return true;
		}

		public:
			static constexpr bool value = _value();
	};

	template <typename MutableTree, typename Left, typename Right>
	error __encode_pair(Left &&left, Right &&right,
			MutableTree &tree
			)
	{
		if constexpr (is_pure_value<Left>::value)
		{
			if (auto err = encode(std::forward<Left>(left), tree))
				return err;

			if constexpr (is_pure_container<Right>::value)
			{
				if (auto err = encode(std::forward<Right>(right), tree))
					return err;
			}
			else
			{
				tree.child_count(1);
				if (auto err = encode(std::forward<Right>(right), tree.child(0)))
					return err;
			}
		}
		else if constexpr (is_pure_value<Right>::value)
		{
			if (auto err = encode(std::forward<Right>(right), tree))
				return err;

			if constexpr (is_pure_container<Left>::value)
			{
				if (auto err = encode(std::forward<Left>(left), tree))
					return err;
			}
			else
			{
				tree.child_count(1);
				if (auto err = encode(std::forward<Left>(left), tree.child(0)))
					return err;
			}
		}
		else
		{
			tree.child_count(2);
			if (auto err = encode(std::forward<Left>(left), tree.child(0)))
				return err;

			if (auto err = encode(std::forward<Right>(right), tree.child(1)))
				return err;
		}

		return {};
	}

	template <typename Tree, typename Left, typename Right>
	error __decode_pair(Tree &&tree, Left &left, Right &right)
	{
		if constexpr (is_pure_value<Left>::value)
		{
			if (auto err = decode(std::forward<Tree>(tree), left))
				return err;

			if constexpr (is_pure_container<Right>::value)
			{
				if (auto err = decode(std::forward<Tree>(tree), right))
					return err;
			}
			else
			{
				if (auto err = decode(tree.child(0), right))
					return err;
			}
		}
		else if constexpr (is_pure_value<Right>::value)
		{
			if (auto err = decode(std::forward<Tree>(tree), right))
				return err;

			if constexpr (is_pure_container<Left>::value)
			{
				if (auto err = decode(std::forward<Tree>(tree), left))
					return err;
			}
			else
			{
				if (auto err = decode(std::forward<Tree>(tree.child(0)), left))
					return err;
			}
		}
		else
		{
			if (auto err = decode(std::forward<Tree>(tree.child(0)), left))
				return err;

			if (auto err = decode(std::forward<Tree>(tree.child(1)), right))
				return err;
		}

		return {};
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename MutableTree,
		std::enable_if_t<index == size, int> = 0
	>
	error __encode_seq(SeqIt &it, MutableTree &tr)
	{
		return {};
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename MutableTree,
		std::enable_if_t<index < size, int> = 0
	>
	error __encode_seq(SeqIt &it, MutableTree &tr)
	{
		using namespace boost::fusion;
		using result_t = typename result_of::value_of<SeqIt>::type; 
		result_t &&x = std::move(*it);

		if (auto err = encode(std::forward<result_t>(x), tr.child(index)))
			return err;

		return __encode_seq<size, index+1>(next(it), tr);
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename Tree,
		std::enable_if_t<index == size, int> = 0
	>
	error __decode_seq(Tree &&tree, SeqIt &it)
	{
		return {};
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename Tree,
		std::enable_if_t<index < size, int> = 0
	>
	error __decode_seq(Tree &&tree, SeqIt &it)
	{
		using namespace boost::fusion;
		typename result_of::value_of<SeqIt>::type x{};

		if (auto err = decode(std::move(tree.child(index)), x))
			return err;

		*it = std::move(x);

		return __decode_seq<size, index+1>(tree, next(it));
	}

	template <
		typename MutableTree,
		typename Sequence,
		__enable_if_seq_t<Sequence>* = nullptr
						 >
	error encode(Sequence &&seq, MutableTree &tree)
	{
		using namespace boost::fusion;
		constexpr auto n = result_of::size<Sequence>::value;

		auto first = begin(seq);

		if constexpr (n == 1)
		{
			using first_type =
				typename result_of::begin<Sequence>::type;

			using result_t = typename result_of::value_of<first_type>::type;
			result_t &&f = std::move(*first);

			return encode(std::forward<result_t>(f), tree);
		}
		else if constexpr (n == 2)
		{
			using first_type =
				typename result_of::begin<Sequence>::type;

			using one_t = typename result_of::value_of<first_type>::type;
			one_t &&f = std::move(*first);

			using second_type =
				typename result_of::next<first_type>::type;

			auto second = next(first);

			using two_t = typename result_of::value_of<second_type>::type;
			two_t &&s = std::move(*second);

			return __encode_pair(std::forward<one_t>(f), std::forward<two_t>(s), tree);
		}
		else
		{
			tree.child_count(n);
			return __encode_seq<n, 0>(first, tree);
		}
	}

	template <
		typename Tree,
		typename Sequence,
		__enable_if_seq_t<Sequence>* = nullptr
						 >
	error decode(const Tree &tree, Sequence &seq)
	{
		using namespace boost::fusion;
		constexpr auto n = result_of::size<Sequence>::value;
		auto first = begin(seq);

		using first_type =
			typename result_of::begin<Sequence>::type;

		if constexpr (n == 1)
		{
			typename result_of::value_of<first_type>::type f{};
			if (auto err = decode(tree, f))
				return err;

			*first = std::move(f);
		}
		else if constexpr (n == 2)
		{
			auto second = next(first);

			typename result_of::value_of<first_type>::type f{};
			using second_type =
				typename result_of::next<first_type>::type;

			typename result_of::value_of<second_type>::type s{};

			if (auto err = __decode_pair(tree, f, s))
				return err;

			*first = std::move(f);
			*second = std::move(s);
		}
		else
		{
			return __decode_seq<n, 0>(tree, first);
		}

		return {};
	}
}

#endif
