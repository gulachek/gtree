#ifndef GULACHEK_GTREE_ENCODING_UNSIGNED_HPP
#define GULACHEK_GTREE_ENCODING_UNSIGNED_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <cstdint>
#include <list>

namespace gulachek::gtree
{
	// Unsigned integers don't use children
	template <typename T>
	struct uses_children<
		T,
		typename std::enable_if<std::is_unsigned_v<T>, void*>::type
	> : std::false_type {};

	// Unsigned Integers are in little endian format
	template <
		typename MutableTree, 
		typename T,
		std::enable_if_t<std::is_unsigned<T>::value, int> = 0
			>
	void encode(const T &val, MutableTree &tree)
	{
		auto n = val;
		std::list<std::uint8_t> bytes;

		while (n > 0)
		{
			bytes.push_back(n);
			n >>= 8;
		}

		tree.value(bytes.begin(), bytes.end());
	}

	template <
		typename Tree, 
		typename T,
		std::enable_if_t<std::is_unsigned<T>::value, int> = 0
			>
	void decode(const Tree &t, T &n)
	{
		n = 0;

		auto tval = t.value();
		auto start = tval.data();
		for (std::size_t i = 0; i < tval.size(); i++)
		{
			T b = start[i];
			n |= (b << 8*i);
		}
	}
}

#endif
