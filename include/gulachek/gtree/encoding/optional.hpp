#ifndef GULACHEK_GTREE_ENCODING_OPTIONAL_HPP
#define GULACHEK_GTREE_ENCODING_OPTIONAL_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include <optional>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<std::optional<T>> : std::false_type {};

	template <typename T>
	struct uses_children<std::optional<T>> : std::true_type {};

	template <typename Tree, typename T>
	error encode(std::optional<T> &&val, Tree &tree)
	{
		if (val)
		{
			tree.child_count(1);
			return encode(std::forward<T>(*val), tree.child(0));
		}
		else
		{
			tree.child_count(0);
			return {};
		}
	}

	template <typename Tree, typename T>
	error decode(Tree &&tree, std::optional<T> &val)
	{
		if (tree.child_count())
		{
			T inner;
			if (auto err = decode(std::move(tree.child(0)), inner))
				return err;

			val = std::move(inner);
		}
		else
		{
			val = std::nullopt;
		}

		return {};
	}
}

#endif
