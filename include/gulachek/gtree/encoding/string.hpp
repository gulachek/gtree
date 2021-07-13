#ifndef GULACHEK_GTREE_ENCODING_STRING_HPP
#define GULACHEK_GTREE_ENCODING_STRING_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <string>

namespace gulachek::gtree
{
	template <>
	struct uses_value<std::decay_t<std::string>> : std::true_type {};

	template <>
	struct uses_children<std::decay_t<std::string>> : std::false_type {};

	// Strings are simply strings of bytes
	// Not generalized to basic_string because of encoding issues
	template <typename MutableTree>
	error encode(std::string &&val, MutableTree &tree)
	{
		tree.value(val.begin(), val.end());
		return {};
	}

	template <typename Tree>
	error decode(Tree &&tree, std::string &val)
	{
		auto tval = tree.value();
		auto start = tval.data();

		val = std::string{start, start + tval.size()};

		return {};
	}
}

#endif
