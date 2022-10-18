#ifndef GULACHEK_GTREE_ENCODING_FUSION_HPP
#define GULACHEK_GTREE_ENCODING_FUSION_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <tuple>
#include <type_traits>

#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/adapted/std_tuple.hpp>

namespace gulachek::gtree
{
	template <typename Sequence>
		requires boost::fusion::traits::is_sequence<Sequence>::value
	struct decoding<Sequence>
	{
		Sequence *p;

		error decode(treeder &r)
		{
			using namespace boost::fusion;
			constexpr auto n = result_of::size<Sequence>::value;
			auto first = begin(*p);

			auto cc = r.child_count();

			if (cc < n)
			{
				error err;
				err << "too few elements in tuple. expected " << n <<
					" but encountered " << cc;
				return err;
			}

			return __decode_seq<n, 0>(r, first);
		}

		template <
			std::size_t size,
			std::size_t index,
			typename SeqIt
		>
			requires (index == size)
		static error __decode_seq(treeder &r, SeqIt &it)
		{
			return {};
		}

		template <
			std::size_t size,
			std::size_t index,
			typename SeqIt
		>
			requires (index < size)
		static error __decode_seq(treeder &r, SeqIt &it)
		{
			using namespace boost::fusion;
			typename result_of::value_of<SeqIt>::type x;

			if (auto err = r.read(&x))
			{
				return err.wrap() << "error reading fusion elem " << index;
			}

			*it = std::move(x);
			return __decode_seq<size, index+1>(r, next(it));
		}
	};

	template <typename Sequence>
		requires boost::fusion::traits::is_sequence<Sequence>::value
	struct encoding<Sequence>
	{
		const Sequence &s;

		error encode(tree_writer &w)
		{
			using namespace boost::fusion;

			constexpr auto n = result_of::size<Sequence>::value;

			w.value(nullptr, 0);
			w.child_count(n);

			auto first = begin(s);
			return __encode_seq<n, 0>(first, w);
		}

		template <
			std::size_t size,
			std::size_t index,
			typename SeqIt
		>
			requires (index == size)
		static error __encode_seq(SeqIt &it, tree_writer &w)
		{
			return {};
		}

		template <
			std::size_t size,
			std::size_t index,
			typename SeqIt
		>
			requires (index < size)
		static error __encode_seq(SeqIt &it, tree_writer &w)
		{
			using namespace boost::fusion;
			using result_t = typename result_of::value_of<SeqIt>::type; 
			result_t x = *it;

			if (auto err = w.write(x))
			{
				return err.wrap() << "error encoding fusion elem " << index;
			}

			return __encode_seq<size, index+1>(
					next(std::forward<SeqIt>(it)), w);
		}
	};
}

#endif
