#ifndef GULACHEK_GTREE_ENCODING_SIGNED_HPP
#define GULACHEK_GTREE_ENCODING_SIGNED_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/twos_complement.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
		requires std::is_signed_v<T>
	struct decoding<T>
	{
		T *pn;

		cause decode(treeder &r)
		{
			auto val = r.value();
			auto nbytes = val.size();
			constexpr auto width = sizeof(T);

			if (nbytes == 0)
			{
				*pn = 0;
				return {};
			}
			else if (nbytes > width)
			{
				cause err;
				err << "Signed integer overflow. " << nbytes << " byte(s) > "
					<< width << " byte(s)";
				return err;
			}


			auto start = val.data();
			bool is_neg = start[nbytes-1] & 0x80;

			// the power of positive thinking
			std::vector<std::uint8_t> neg_buf;
			if (is_neg)
			{
				neg_buf = std::vector<std::uint8_t>{start, start + nbytes};
				twos_complement(neg_buf.data(), neg_buf.size());
				start = neg_buf.data();
			}
			
			std::size_t base;
			if (auto err = decode_unsigned(start, nbytes, &base))
				return err;

			*pn = base;
			if (is_neg) *pn = -*pn;

			return {};
		}
	};

	template <typename T>
		requires std::is_signed_v<T>
	struct encoding<T>
	{
		const T &n;

		cause encode(tree_writer &w)
		{
			bool is_neg = (n < 0);
			std::uint8_t buf[sizeof(T)];
			std::size_t pos = is_neg ? -n : n;
			auto nbytes = encode_unsigned(buf, pos);
			if (is_neg)
				twos_complement(buf, nbytes);

			w.value(buf, nbytes);
			w.child_count(0);
			return {};
		}
	};
}

#endif
