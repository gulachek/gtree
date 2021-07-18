#ifndef GULACHEK_GTREE_ENCODING_UNSIGNED_HPP
#define GULACHEK_GTREE_ENCODING_UNSIGNED_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <cstdint>
#include <list>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct encoding<
		T, 
		typename std::enable_if<std::is_unsigned_v<std::decay_t<T>>, void>::type
		>
	{
		using type = T;

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = false;

		template <typename U, typename MutableTree>
		static error encode(U &&val, MutableTree &tree)
		{
			auto n = val;
			std::list<std::uint8_t> bytes;

			while (n > 0)
			{
				bytes.push_back(n);

				// avoid compiler warning for width of type here
				if constexpr (sizeof(T) == 1) n = 0;
				else n >>= 8;
			}

			tree.value(bytes.begin(), bytes.end());

			return {};
		}

		template <typename Tree>
		static error decode(Tree &&t, T &n)
		{
			if (t.value().size() > sizeof(std::decay_t<T>))
				return "Integer too big";

			n = 0;

			auto tval = t.value();
			auto start = tval.data();
			for (std::size_t i = 0; i < tval.size(); i++)
			{
				T b = start[i];
				n |= (b << 8*i);
			}

			return {};
		}
	};
}

#endif
