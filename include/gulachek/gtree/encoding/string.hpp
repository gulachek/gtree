#ifndef GULACHEK_GTREE_ENCODING_STRING_HPP
#define GULACHEK_GTREE_ENCODING_STRING_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <string>
#include <type_traits>

namespace gulachek::gtree
{
	template <>
	struct encoding<std::string>
	{
		using type = std::string;

		static constexpr bool uses_value = true;
		static constexpr bool uses_children = false;

		// Strings are simply strings of bytes
		// Not generalized to basic_string because of encoding issues
		template <typename String, typename MutableTree>
		static error encode(String &&val, MutableTree &tree)
		{
			tree.value({(std::uint8_t*)val.data(), val.size()});
			return {};
		}

		template <typename Tree>
		static error decode(Tree &&tree, std::string &val)
		{
			auto tval = tree.value();
			auto start = tval.data();

			val = std::string{start, start + tval.size()};

			return {};
		}
	};
}

#endif
