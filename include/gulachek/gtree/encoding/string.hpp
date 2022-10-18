#ifndef GULACHEK_GTREE_ENCODING_STRING_HPP
#define GULACHEK_GTREE_ENCODING_STRING_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <string>
#include <type_traits>

namespace gulachek::gtree
{
	template <>
	struct decoding<std::string>
	{
		std::string *s;

		error decode(treeder &r)
		{
			auto val = r.value();
			auto start = (const char*) val.data();
			*s = std::string{start, start + val.size()};
			return {};
		}
	};

	template <>
	struct encoding<std::string>
	{
		const std::string &s;

		error encode(tree_writer &w)
		{
			w.value(s.data(), s.size());
			w.child_count(0);
			return {};
		}
	};
}

#endif
