#ifndef GULACHEK_GTREE_BASE128_HPP
#define GULACHEK_GTREE_BASE128_HPP

#include <gulachek/cause.hpp>

#include <istream>
#include <cstddef>

namespace gulachek::gtree
{
	cause read_num(std::istream &is, std::size_t *n)
	{
		*n = 0;
		std::size_t power = 1;
		std::size_t b;

		auto c = is.get();
		auto eof = std::istream::traits_type::eof();

		if (c == eof)
		{
			if (is.eof())
				return cause::eof();
			else
				return {"bad stream"};
		}

		b = (std::size_t) c;
		while (b & 0x80)
		{
			*n += (power * (b & 0x7f));
			power *= 128;

			c = is.get();
			if (c == eof)
				return {"incomplete base 127 integer"};

			b = (std::size_t) c;
		}

		*n += (power * b);
		return {};
	}

}

#endif
