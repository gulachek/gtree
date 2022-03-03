#ifndef GULACHEK_GTREE_ENCODING_TUPLE_HPP
#define GULACHEK_GTREE_ENCODING_TUPLE_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <tuple>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename ...Ts>
	struct decoding<std::tuple<Ts...>>
	{
		static constexpr std::size_t n = sizeof...(Ts);

		std::tuple<Ts...> *ptup;

		cause decode(treeder &r)
		{
			auto cc = r.child_count();
			if (cc < n)
			{
				cause err;
				err << "too few tuple elements to decode. expected " <<
					n << " but encountered " << cc;
				return err;
			}

			return decode_elem<0>(r);
		}

		template <std::size_t index>
			requires (index == n)
		cause decode_elem(treeder &r)
		{
			return {};
		}

		template <std::size_t index>
			requires (index < n)
		cause decode_elem(treeder &r)
		{
			if (auto err = r.read(&std::get<index>(*ptup)))
			{
				cause wrapper;
				wrapper << "error decoding tuple elem " << index;
				wrapper.add_cause(err);
				return err;
			}

			return decode_elem<index+1>(r);
		}
	};

	template <typename ...Ts>
	struct encoding<std::tuple<Ts...>>
	{
		static constexpr std::size_t n = sizeof...(Ts);

		const std::tuple<Ts...> &tup;

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(n);
			return encode_elem<0>(w);
		}

		template <std::size_t index>
			requires (index == n)
		cause encode_elem(tree_writer &)
		{
			return {};
		}

		template <std::size_t index>
			requires (index < n)
		cause encode_elem(tree_writer &w)
		{
			if (auto err = w.write(std::get<index>(tup)))
			{
				cause wrapper;
				wrapper << "error encoding tuple elem " << index;
				wrapper.add_cause(err);
				return wrapper;
			}

			return encode_elem<index+1>(w);
		}
	};
}

#endif
