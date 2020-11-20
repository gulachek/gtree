#ifndef GULACHEK_GTREE_ENCODING_FUSION_HPP
#define GULACHEK_GTREE_ENCODING_FUSION_HPP

#include <tuple>
#include <type_traits>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>
#include <boost/fusion/include/std_tuple.hpp>

namespace gulachek::gtree
{
	template <typename T, typename U>
	struct __enable_if_seq : std::enable_if<
			boost::fusion::traits::is_sequence<T>::value, U> {};

	template <typename T, typename U>
	using __enable_if_seq_t =
		typename __enable_if_seq<T, U>::type;

	template <typename T>
	struct uses_value<
		T,
		__enable_if_seq_t<T, void*>
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
		__enable_if_seq_t<T, void*>
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
	void __encode_pair(const Left &left, const Right &right,
			MutableTree &tree
			)
	{
		if constexpr (is_pure_value<Left>::value)
		{
			encode(left, tree);

			if constexpr (is_pure_container<Right>::value)
			{
				encode(right, tree);
			}
			else
			{
				tree.child_count(1);
				encode(right, tree.child(0));
			}
		}
		else if constexpr (is_pure_value<Right>::value)
		{
			encode(right, tree);

			if constexpr (is_pure_container<Left>::value)
			{
				encode(left, tree);
			}
			else
			{
				tree.child_count(1);
				encode(left, tree.child(0));
			}
		}
		else
		{
			tree.child_count(2);
			encode(left, tree.child(0));
			encode(right, tree.child(1));
		}
	}

	template <typename Tree, typename Left, typename Right>
	void __decode_pair(const Tree &tree, Left &left, Right &right)
	{
		if constexpr (is_pure_value<Left>::value)
		{
			decode(tree, left);

			if constexpr (is_pure_container<Right>::value)
				decode(tree, right);
			else
				decode(tree.child(0), right);
		}
		else if constexpr (is_pure_value<Right>::value)
		{
			decode(tree, right);

			if constexpr (is_pure_container<Left>::value)
				decode(tree, left);
			else
				decode(tree.child(0), left);
		}
		else
		{
			decode(tree.child(0), left);
			decode(tree.child(1), right);
		}
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename MutableTree,
		std::enable_if_t<index == size, int> = 0
	>
	void __encode_seq(const SeqIt &it, MutableTree &tr)
	{}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename MutableTree,
		std::enable_if_t<index < size, int> = 0
	>
	void __encode_seq(const SeqIt &it, MutableTree &tr)
	{
		using namespace boost::fusion;
		const typename result_of::value_of<SeqIt>::type &x = *it;
		encode(x, tr.child(index));
		__encode_seq<size, index+1>(next(it), tr);
	}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename Tree,
		std::enable_if_t<index == size, int> = 0
	>
	void __decode_seq(const Tree &tree, SeqIt &it)
	{}

	template <
		std::size_t size,
		std::size_t index,
		typename SeqIt,
		typename Tree,
		std::enable_if_t<index < size, int> = 0
	>
	void __decode_seq(const Tree &tree, SeqIt &it)
	{
		using namespace boost::fusion;
		typename result_of::value_of<SeqIt>::type x{};

		decode(tree.child(index), x);
		*it = std::move(x);

		__decode_seq<size, index+1>(tree, next(it));
	}

	template <
		typename MutableTree,
		typename Sequence,
		__enable_if_seq_t<Sequence, int> = 0
						 >
	void encode(const Sequence &seq, MutableTree &tree)
	{
		using namespace boost::fusion;
		constexpr auto n = result_of::size<Sequence>::value;

		const auto first = begin(seq);

		if constexpr (n == 1)
		{
			using first_type =
				typename result_of::begin<Sequence>::type;

			const typename result_of::value_of<first_type>::type
				&f = *first;

			encode(f, tree);
		}
		else if constexpr (n == 2)
		{
			using first_type =
				typename result_of::begin<Sequence>::type;

			const typename result_of::value_of<first_type>::type
				&f = *first;

			using second_type =
				typename result_of::next<first_type>::type;

			auto second = next(first);

			const typename result_of::value_of<second_type>::type
				&s = *second;

			__encode_pair(f, s, tree);
		}
		else
		{
			tree.child_count(n);
			__encode_seq<n, 0>(first, tree);
		}
	}

	template <
		typename Tree,
		typename Sequence,
		__enable_if_seq_t<Sequence, int> = 0
						 >
	void decode(const Tree &tree, Sequence &seq)
	{
		using namespace boost::fusion;
		constexpr auto n = result_of::size<Sequence>::value;
		auto first = begin(seq);

		using first_type =
			typename result_of::begin<Sequence>::type;

		if constexpr (n == 1)
		{
			typename result_of::value_of<first_type>::type f{};
			decode(tree, f);
			*first = std::move(f);
		}
		else if constexpr (n == 2)
		{
			auto second = next(first);

			typename result_of::value_of<first_type>::type f{};
			using second_type =
				typename result_of::next<first_type>::type;

			typename result_of::value_of<second_type>::type s{};

			__decode_pair(tree, f, s);

			*first = std::move(f);
			*second = std::move(s);
		}
		else
		{
			__decode_seq<n, 0>(tree, first);
		}
	}
}

#endif
