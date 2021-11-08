#ifndef GULACHEK_GTREE_ENCODING_SIGNED_HPP
#define GULACHEK_GTREE_ENCODING_SIGNED_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/twos_complement.hpp"
#include <type_traits>

namespace gulachek::gtree
{
	template <typename U>
	struct encoding<U, 
		std::enable_if_t<std::is_signed_v<std::decay_t<U>>, void>
			>
	{
		using type = U;

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = false;

		template <typename Tree>
		static error decode(Tree &&t, type &n)
		{
			n = 0;

			auto val = t.value();
			auto nbytes = val.size();
			constexpr auto width = sizeof(type);

			if (nbytes == 0)
			{
				return {};
			}
			else if (nbytes > width)
			{
				error err;
				err << "Signed integer overflow. " << nbytes << " byte(s) > "
					<< width << " byte(s)";
				return err;
			}


			auto start = val.data();
			bool is_neg = start[nbytes-1] & 0x80;

			// the power of positive thinking
			mutable_tree pos;
			pos.value({start, nbytes});
			if (is_neg) twos_complement(pos.value());
			
			std::size_t base;
			gtree::decode(pos, base);
			n = base;

			if (is_neg) n = -n;

			return {};
		}

		template <typename Tree, typename T>
		static error encode(T &&n, Tree &t)
		{
			bool is_neg = (n < 0);

			std::size_t pos = is_neg ? -n : n;
			gtree::encode(pos, t);
			if (is_neg) twos_complement(t.value());

			return {};
		}
	};
}

#endif
