#ifndef GULACHEK_GTREE_ENCODING_EMPTY_HPP
#define GULACHEK_GTREE_ENCODING_EMPTY_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	// Annotate your empty class with a type member
	// This is more intrusive than it seems to enable for all
	// std::is_empty, especially for contrived tests
	template <typename T, typename Enable = void>
	struct empty_encoding {};

	template <typename U>
	struct encoding<U, enable_t<typename empty_encoding<U>::type>>
	{
		using type = U;

		template <typename T, typename MutableTree>
		static error encode(T &&val, MutableTree &tree)
		{
			tree = MutableTree{};
			return {};
		}

		template <typename Tree, typename T>
		static error decode(Tree &&tr, T &val)
		{
			return {};
		}
	};
}

#endif
