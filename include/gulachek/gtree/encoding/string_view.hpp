#ifndef GULACHEK_GTREE_ENCODING_STRING_VIEW_HPP
#define GULACHEK_GTREE_ENCODING_STRING_VIEW_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <string_view>
#include <type_traits>

namespace gulachek::gtree
{
	template <>
	struct encoding<std::string_view>
	{
		const std::string_view &s;

		cause encode(tree_writer &w)
		{
			w.value(s.data(), s.size());
			w.child_count(0);
			return {};
		}
	};
}

#endif
