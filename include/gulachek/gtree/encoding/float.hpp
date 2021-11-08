#ifndef GULACHEK_GTREE_ENCODING_FLOAT_HPP
#define GULACHEK_GTREE_ENCODING_FLOAT_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <type_traits>
#include <cmath>

namespace gulachek::gtree
{
	template <>
	struct encoding<float, void>
	{
		using type = float;

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = false;

		template <typename MutableTree>
		static error encode(float f, MutableTree &tree)
		{
			// Naive implementation... optimize by compiler later
			auto type = std::fpclassify(f);

			std::uint8_t b[4] = {0,0,0,0};

			if (std::signbit(f))
			{
				f *= -1.0f;
				b[3] = 0x80u;
			}

			if (type == FP_INFINITE)
			{
				b[3] |= 0x7f;
				b[2] = 0x80;
			}
			else if (type == FP_NAN)
			{
				b[3] = 0x7f;
				b[2] = 0x80;
				b[0] = 1;
			}
			else if (type == FP_NORMAL)
			{
				int exp;
				auto frac = std::frexp(f, &exp);

				// convert frac from [0.5, 1.0) to 1.bbbbb
				frac *= 2.0f;
				exp -= 1;

				// encode E where exp == E - 127
				exp += 127;
				b[3] |= ((exp & 0xfe) >> 1);
				b[2] |= ((exp & 1) << 7);

				int mantissa = std::ldexp(frac - 1.0f, 23);
				for (std::size_t i = 0; i < 23; ++i)
				{
					auto byte = i / 8;
					auto bit = i % 8;
					if (mantissa & (1 << i))
						b[byte] |= (1 << bit);
				}
			}
			else if (type == FP_SUBNORMAL)
			{
				int exp;
				auto frac = std::frexp(f, &exp);
				while (exp < -126)
				{
					frac /= 2.0f;
					exp += 1;
				}
				while (exp > -126)
				{
					frac *= 2.0f;
					exp -= 1;
				}

				if (frac >= 1.0f)
					return {"Subnormal float is too big"};

				int mantissa = (int)std::ldexp(frac, 23);
				for (std::size_t i = 0; i < 23; ++i)
				{
					auto byte = i / 8;
					auto bit = i % 8;
					if (mantissa & (1 << i))
						b[byte] |= (1 << bit);
				}
			}
			else if (type == FP_ZERO)
			{
				// no work necessary
			}
			else
			{
				return {"Unknown float type"};
			}

			block blk{b, 4};
			tree.value(blk);
			return {};
		}

		template <typename Tree>
		static error decode(Tree &&t, float &f)
		{
			// Naive implementation... optimize by compiler later
			auto value = t.value();
			if (value.size() != 4)
			{
				error err;
				err << "Expected float to have 4 bytes. Tree had " << value.size();
				return err;
			}

			auto b = value.data();

			float sign = (0x80 & b[3]) ? -1.0f : 1.0f;
			int exp =
				((0x7f & b[3]) << 1) |
				((0x80 & b[2]) >> 7);

			unsigned long long fraction = 0;
			fraction =
				b[0] |
				(b[1] << 8) |
				((b[2] & 0x7f) << 16);

			float x = 0;
			for (std::size_t i = 0; i < 23; ++i)
			{
				if (fraction & (1 << i))
					x += 1.0f;
				x /= 2.0f;
			}

			if (exp == 0)
			{
				if (fraction == 0.0f)
				{
					f = sign * 0.0f;
				}
				else
				{
					f = sign * std::ldexp(x, -126);
				}
			}
			else if (exp == 0xff)
			{
				if (fraction == 0.0f)
				{
					f = sign * INFINITY;
				}
				else
				{
					f = std::nanf("");
				}
			}
			else
			{
				f = sign * std::ldexp(1 + x, exp - 127);
			}

			return {};
		}
	};
}

#endif
