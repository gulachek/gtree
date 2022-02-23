#ifndef GULACHEK_GTREE_DECODING_HPP
#define GULACHEK_GTREE_DECODING_HPP

#include "gulachek/gtree/child_reader.hpp"
#include "gulachek/gtree/my_same_as.hpp"

#include <gulachek/cause.hpp>

#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct decoding {};

	template <typename T>
	concept decodable = requires
		(
		 T *p,
		 decoding<T> dec,
		 std::size_t n,
		 std::uint8_t *data,
		 child_reader r
		 )
	{
		typename decoding<T>;
		requires std::is_default_constructible_v<T>;
		requires std::is_constructible_v<decoding<T>, T*>;

		{dec.value(n, data)} -> my_same_as<cause>;
		{dec.children(n, r)} -> my_same_as<cause>;
	};

}

#endif
