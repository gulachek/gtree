#ifndef GULACHEK_GTREE_ENCODING_UNSIGNED_HPP
#define GULACHEK_GTREE_ENCODING_UNSIGNED_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <cstdint>
#include <cmath>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
		requires std::is_unsigned_v<T>
	struct decoding<T>
	{
		T *pn;

		cause decode(treeder &r)
		{
			auto tval = r.value();
			auto size = tval.size();

			constexpr auto width = sizeof(T);
			if (size > width)
			{
				cause err;
				err << "Unsigned integer overflow. '" << size << "' cannot fit in '"
					<< width << "' byte(s)";
				return err;
			}

			T n = 0;

			auto start = tval.data();
			for (std::size_t i = 0; i < size; i++)
			{
				T b = start[i];
				n |= (b << 8*i);
			}

			*pn = n;
			return {};
		}
	};

	template <typename T>
		requires std::is_unsigned_v<T>
	struct encoding<T>
	{
		const T &n;

		cause encode(tree_writer &w)
		{
			std::uint8_t bytes[sizeof(T)];
			std::size_t i = 0;

			auto x = n;
			while (x > 0)
			{
				bytes[i++] = (x % 256);
				x /= 256;
			}

			w.value(bytes, i);
			w.child_count(0);
			return {};
		}
	};
}

#endif
