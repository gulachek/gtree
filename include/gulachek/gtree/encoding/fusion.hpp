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
	class encoding<T, std::enable_if_t<boost::fusion::traits::is_sequence<T>::value, void>>
	{
		private:
			static constexpr bool uses_value_()
			{
				using namespace boost::fusion::result_of;

				constexpr auto n = size<T>::value;

				if constexpr (n == 1)
				{
					using first = typename begin<T>::type;
					using t = typename value_of<first>::type;
					return gtree::uses_value<t>::value;
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

			static constexpr bool uses_children_()
			{
				using namespace boost::fusion::result_of;

				constexpr auto n = size<T>::value;

				if constexpr (n == 1)
				{
					using first = typename begin<T>::type;
					using t = typename value_of<first>::type;
					return gtree::uses_children<t>::value;
				}

				return true;
			}

			template <typename MutableTree, typename Left, typename Right>
			static error __encode_pair(Left &&left, Right &&right,
					MutableTree &tree
					)
			{
				if constexpr (is_pure_value<Left>::value)
				{
					if (auto err = gtree::encode(std::forward<Left>(left), tree))
						return err;

					if constexpr (is_pure_container<Right>::value)
					{
						if (auto err = gtree::encode(std::forward<Right>(right), tree))
							return err;
					}
					else
					{
						tree.child_count(1);
						if (auto err = gtree::encode(std::forward<Right>(right), tree.child(0)))
							return err;
					}
				}
				else if constexpr (is_pure_value<Right>::value)
				{
					if (auto err = gtree::encode(std::forward<Right>(right), tree))
						return err;

					if constexpr (is_pure_container<Left>::value)
					{
						if (auto err = gtree::encode(std::forward<Left>(left), tree))
							return err;
					}
					else
					{
						tree.child_count(1);
						if (auto err = gtree::encode(std::forward<Left>(left), tree.child(0)))
							return err;
					}
				}
				else
				{
					tree.child_count(2);
					if (auto err = gtree::encode(std::forward<Left>(left), tree.child(0)))
						return err;

					if (auto err = gtree::encode(std::forward<Right>(right), tree.child(1)))
						return err;
				}

				return {};
			}

			template <typename Tree, typename Left, typename Right>
			static error __decode_pair(Tree &&tree, Left &left, Right &right)
			{
				if constexpr (is_pure_value<Left>::value)
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree), left))
						return err;

					if constexpr (is_pure_container<Right>::value)
					{
						if (auto err = gtree::decode(std::forward<Tree>(tree), right))
							return err;
					}
					else
					{
						if (auto err = gtree::decode(tree.child(0), right))
							return err;
					}
				}
				else if constexpr (is_pure_value<Right>::value)
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree), right))
						return err;

					if constexpr (is_pure_container<Left>::value)
					{
						if (auto err = gtree::decode(std::forward<Tree>(tree), left))
							return err;
					}
					else
					{
						if (auto err = gtree::decode(std::forward<Tree>(tree.child(0)), left))
							return err;
					}
				}
				else
				{
					if (auto err = gtree::decode(std::forward<Tree>(tree.child(0)), left))
						return err;

					if (auto err = gtree::decode(std::forward<Tree>(tree.child(1)), right))
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
			static error __encode_seq(SeqIt &it, MutableTree &tr)
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
			static error __encode_seq(SeqIt &it, MutableTree &tr)
			{
				using namespace boost::fusion;
				using result_t = typename result_of::value_of<SeqIt>::type; 
				result_t &&x = std::move(*it);

				if (auto err = gtree::encode(std::forward<result_t>(x), tr.child(index)))
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
			static error __decode_seq(Tree &&tree, SeqIt &it)
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
			static error __decode_seq(Tree &&tree, SeqIt &it)
			{
				using namespace boost::fusion;
				typename result_of::value_of<SeqIt>::type x{};

				if (auto err = gtree::decode(std::move(tree.child(index)), x))
					return err;

				*it = std::move(x);

				return __decode_seq<size, index+1>(tree, next(it));
			}

		public:
			using type = T;
			static constexpr bool uses_value = uses_value_();
			static constexpr bool uses_children = uses_children_();

			template <typename MutableTree, typename Sequence>
			static error encode(Sequence &&seq, MutableTree &tree)
			{
				using seq_t = std::decay_t<Sequence>;
				using namespace boost::fusion;
				constexpr auto n = result_of::size<seq_t>::value;

				auto first = begin(seq);

				if constexpr (n == 1)
				{
					using first_type =
						typename result_of::begin<seq_t>::type;

					using result_t = typename result_of::value_of<first_type>::type;
					result_t &&f = std::move(*first);

					return gtree::encode(std::forward<result_t>(f), tree);
				}
				else if constexpr (n == 2)
				{
					using first_type =
						typename result_of::begin<seq_t>::type;

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

			template <typename Tree, typename Sequence>
			static error decode(const Tree &tree, Sequence &seq)
			{
				using namespace boost::fusion;
				constexpr auto n = result_of::size<Sequence>::value;
				auto first = begin(seq);

				using first_type =
					typename result_of::begin<Sequence>::type;

				if constexpr (n == 1)
				{
					typename result_of::value_of<first_type>::type f{};
					if (auto err = gtree::decode(tree, f))
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

	};
}

#endif
