#ifndef GULACHEK_GTREE_ENCODING_UNSIGNED_HPP
#define GULACHEK_GTREE_ENCODING_UNSIGNED_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <cstdint>
#include <cmath>
#include <span>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
		requires std::is_unsigned_v<T>
	cause decode_unsigned(const void *buf, std::size_t nbytes, T *pn)
	{
		auto start = (const std::uint8_t*)buf;
		auto size = nbytes;

		constexpr auto width = sizeof(T);
		if (size > width)
		{
			cause err;
			err << "Unsigned integer overflow. '" << size << "' bytes "
				"cannot fit in '" << width << "' byte(s)";
			return err;
		}

		T n = 0;

		for (std::size_t i = 0; i < size; i++)
		{
			T b = start[i];
			n |= (b << 8*i);
		}

		*pn = n;
		return {};
	}

	template <typename T>
	cause decode_unsigned(
			const std::span<const std::uint8_t> &val, T *pn)
	{
		return decode_unsigned(val.data(), val.size(), pn);
	}

	template <typename T>
		requires std::is_unsigned_v<T>
	struct decoding<T>
	{
		T *pn;

		cause decode(treeder &r)
		{
			auto tval = r.value();
			return decode_unsigned(tval.data(), tval.size(), pn);
		}
	};

	template <typename T>
		requires std::is_unsigned_v<T>
	std::size_t encode_unsigned(void *buf, T n)
	{
		std::size_t i = 0;
		auto bytes = (std::uint8_t*)buf;

		while (n > 0)
		{
			bytes[i++] = (n % 256);
			n /= 256;
		}

		return i;
	}

	template <typename T>
		requires std::is_unsigned_v<T>
	struct encoding<T>
	{
		const T &n;

		cause encode(tree_writer &w)
		{
			std::uint8_t bytes[sizeof(T)];
			auto nbytes = encode_unsigned(bytes, n);
			w.value(bytes, nbytes);
			w.child_count(0);
			return {};
		}
	};
}

#endif
