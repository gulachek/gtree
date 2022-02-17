#ifndef GULACHEK_GTREE_BASE128_HPP
#define GULACHEK_GTREE_BASE128_HPP

#include <gulachek/cause.hpp>

#include <istream>
#include <cstddef>

namespace gulachek::gtree
{
	enum read_base128_error
	{
		incomplete = 1,
		bad_stream,
		overflow
	};

	cause read_base128(std::istream &is, std::size_t *n)
	{
		*n = 0;
		std::size_t power = 1;
		std::size_t b;
		std::size_t nbits = 7;

		auto c = is.get();
		auto eof = std::istream::traits_type::eof();

		if (c == eof)
		{
			if (is.eof())
				return cause::eof();
			else
				return {read_base128_error::bad_stream, "bad stream"};
		}

		b = (std::size_t) c;
		while (b & 0x80)
		{
			*n += (power * (b & 0x7f));
			power *= 128;
			nbits += 7;

			c = is.get();
			if (c == eof)
			{
				return
				{
					read_base128_error::incomplete,
					"incomplete base 127 integer"
				};
			}

			b = (std::size_t) c;

			if ((nbits > 8*sizeof(std::size_t)) && b > 1)
				return {read_base128_error::overflow, "overflow"};

		}

		*n += (power * b);
		return {};
	}
}

#endif
