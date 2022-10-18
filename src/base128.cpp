#include "gulachek/gtree/base128.hpp"

namespace gulachek::gtree
{
	static consteval std::size_t overflow_cap()
	{
		constexpr auto width = sizeof(std::size_t);
		static_assert(width == 4 || width == 8, "Unhandled sizeof(std::size_t)");

		if (width == 4)
		{
			return 0x0f;
		}
		else
		{
			return 0x01;
		}
	}

	error read_base128(std::istream &is, std::size_t *n)
	{
		*n = 0;
		std::size_t power = 1;
		std::size_t b;
		std::size_t nbits = 7;
		constexpr auto cap = overflow_cap();

		auto c = is.get();
		auto eof = std::istream::traits_type::eof();

		if (c == eof)
		{
			if (is.eof())
				return error::eof();
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

			if ((nbits > 8*sizeof(std::size_t)) && b > cap)
				return {read_base128_error::overflow, "overflow"};

		}

		*n += (power * b);
		return {};
	}

	error write_base128(std::ostream &os, std::size_t n)
	{
		while (n > 127)
		{
			auto b = n % 128;
			n /= 128;
			os.put(0x80 | b);
		}

		if (!os.put(n))
			return {write_base128_error::bad_stream, "bad_stream"};

		return {};
	}
}
