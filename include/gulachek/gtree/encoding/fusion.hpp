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
				result_t x = *it;

				if (auto err = gtree::encode(x, tr.child(index)))
					return err;

				return __encode_seq<size, index+1>(
						next(std::forward<SeqIt>(it)), tr);
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

				if (auto err = gtree::decode(
							std::forward<Tree>(tree).child(index), x))
					return err;

				*it = std::move(x);

				return __decode_seq<size, index+1>(tree, next(it));
			}

		public:
			using type = T;

			template <typename MutableTree, typename Sequence>
			static error encode(Sequence &&seq, MutableTree &tree)
			{
				using seq_t = std::decay_t<Sequence>;
				using namespace boost::fusion;
				constexpr auto n = result_of::size<seq_t>::value;

				auto first = begin(seq);

				tree.child_count(n);
				return __encode_seq<n, 0>(first, tree);
			}

			template <typename Tree, typename Sequence>
			static error decode(const Tree &tree, Sequence &seq)
			{
				using namespace boost::fusion;
				constexpr auto n = result_of::size<Sequence>::value;
				auto first = begin(seq);

				if (tree.child_count() < n)
				{
					return {"not enough children"};
				}

				return __decode_seq<n, 0>(tree, first);
			}

	};
}

#endif
