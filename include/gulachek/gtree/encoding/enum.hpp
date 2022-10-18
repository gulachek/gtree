#ifndef GULACHEK_GTREE_ENCODING_ENUM_HPP
#define GULACHEK_GTREE_ENCODING_ENUM_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
		requires std::is_enum_v<T>
	struct decoding<T>
	{
		T *p;

		error decode(treeder &r)
		{
			using underlying_t = std::underlying_type_t<T>;
			decoding<underlying_t> dec{(underlying_t*)p};
			return dec.decode(r);
		}
	};

	template <typename T>
		requires std::is_enum_v<T>
	struct encoding<T>
	{
		const T &val;

		error encode(tree_writer &w)
		{
			using underlying_t = std::underlying_type_t<T>;
			encoding<underlying_t> enc{(const underlying_t&)val};
			return enc.encode(w);
		}
	};
}

#endif
