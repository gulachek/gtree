#ifndef GULACHEK_GTREE_ENCODING_TUPLE_HPP
#define GULACHEK_GTREE_ENCODING_TUPLE_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <tuple>
#include <type_traits>

namespace gulachek::gtree
{
	// class tuples
	template <typename T>
	concept class_tuple = requires
	{
		T::gtree_tuple;
		std::tuple_size<decltype(T::gtree_tuple)>::value;
	};

	template <class_tuple T>
	struct decoding<T>
	{
		using mems_type = decltype(T::gtree_tuple);
		static constexpr std::size_t n = std::tuple_size<mems_type>::value;

		T *ptup;

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
			auto pmem = std::get<index>(T::gtree_tuple);

			if constexpr (std::is_member_object_pointer_v<decltype(pmem)>)
			{
				auto &mem = ptup->*pmem;
				if (auto err = r.read(&mem))
				{
					cause wrapper;
					wrapper << "error decoding tuple elem " << index;
					wrapper.add_cause(err);
					return err;
				}
			}
			else
			{
				return {"gtree_tuple member not supported"};
			}

			return decode_elem<index+1>(r);
		}
	};

	template <class_tuple T>
	struct encoding<T>
	{
		using gtree_tuple = decltype(T::gtree_tuple);
		static constexpr auto n = std::tuple_size<gtree_tuple>::value;

		const T &tup;

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
			auto pmem = std::get<index>(T::gtree_tuple);

			if constexpr (std::is_member_object_pointer_v<decltype(pmem)>)
			{
				if (auto err = w.write(tup.*pmem))
				{
					cause wrapper;
					wrapper << "error encoding tuple elem " << index;
					wrapper.add_cause(err);
					return wrapper;
				}
			}
			else
			{
				return {"gtree_tuple member not supported"};
			}

			return encode_elem<index+1>(w);
		}

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(n);
			return encode_elem<0>(w);
		}
	};

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

	template <typename ...Elems>
	cause read_tuple(treeder &r, Elems *...elems)
	{
		using tuple_type =
			std::tuple<std::add_lvalue_reference_t<Elems>...>;

		tuple_type impl{*elems...};

		decoding<tuple_type> dec{&impl};
		return dec.decode(r);
	}

	template <typename ...Elems>
	cause read_tuple_lvalref(treeder &r, Elems &...elems)
	{
		return read_tuple(r, &elems...);
	}

	template <typename ...Elems>
	cause write_tuple(tree_writer &w, const Elems &...elems)
	{
		using tuple_type = std::tuple<const Elems &...>;
		tuple_type impl{elems...};

		encoding<tuple_type> enc{impl};
		return enc.encode(w);
	}
}

#define GTREE_TUPLE_MEMBERS(...) \
	static constexpr auto gtree_tuple = std::make_tuple(\
			__VA_ARGS__\
			)

#define GTREE_DEFINE_TUPLE_MEMBER_FNS(TYPENAME, ...) \
gulachek::cause TYPENAME ::gtree_decode(gulachek::gtree::treeder &r) \
{ \
	return gulachek::gtree::read_tuple_lvalref(r, __VA_ARGS__); \
} \
\
gulachek::cause TYPENAME ::gtree_encode(gulachek::gtree::tree_writer &w) const \
{ \
	return gulachek::gtree::write_tuple(w, __VA_ARGS__); \
}

#endif
