#ifndef GULACHEK_GTREE_ENCODING_SIGNED_HPP
#define GULACHEK_GTREE_ENCODING_SIGNED_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/twos_complement.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<T,
		std::enable_if_t<std::is_signed_v<std::decay_t<T>>, void>
			> : std::true_type {};

	template <typename T>
	struct uses_children<T,
		std::enable_if_t<std::is_signed_v<std::decay_t<T>>, void>
			> : std::false_type {};

	// two's complement decode
	template <
		typename Tree, typename T,
		std::enable_if_t<std::is_signed_v<std::decay_t<T>>, void*>
			= nullptr
			>
	error decode(Tree &&t, T &n)
	{
		n = 0;

		auto val = t.value();
		auto nbytes = val.size();

		if (nbytes == 0)
			return {};
		else if (nbytes > sizeof(T))
			return "Integer too big";

		auto start = val.data();
		bool is_neg = start[nbytes-1] & 0x80;

		// the power of positive thinking
		mutable_tree pos;
		pos.value(start, start + nbytes);
		if (is_neg) twos_complement(pos.value());
		
		std::size_t base;
		decode(pos, base);
		n = base;

		if (is_neg) n = -n;

		return {};
	}

	// two's complement decode
	template <
		typename Tree, typename T,
		std::enable_if_t<std::is_signed_v<std::decay_t<T>>, void*>
			= nullptr
			>
	error encode(T &&n, Tree &t)
	{
		bool is_neg = (n < 0);

		std::size_t pos = is_neg ? -n : n;
		encode(pos, t);
		if (is_neg) twos_complement(t.value());

		return {};
	}
}

#endif
