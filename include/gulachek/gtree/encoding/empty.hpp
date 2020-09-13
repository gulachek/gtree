#ifndef GULACHEK_GTREE_ENCODING_EMPTY_HPP
#define GULACHEK_GTREE_ENCODING_EMPTY_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	struct empty {};

	template <>
	struct uses_value<empty> : std::false_type {};

	template <>
	struct uses_children<empty> : std::false_type {};

	template <typename MutableTree>
	void encode(const empty &val, MutableTree &tree)
	{
		tree = MutableTree{};
	}

	template <typename Tree>
	void decode(const Tree &tr, empty &val)
	{
	}
}

#endif
