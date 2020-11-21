#ifndef GULACHEK_GTREE_ENCODING_OPTIONAL_HPP
#define GULACHEK_GTREE_ENCODING_OPTIONAL_HPP

#include <optional>

namespace gulachek::gtree
{
	template <typename T>
	struct uses_value<std::optional<T>> : std::false_type {};

	template <typename T>
	struct uses_children<std::optional<T>> : std::true_type {};

	template <typename Tree, typename T>
	void encode(const std::optional<T> &val, Tree &tree)
	{
		if (val)
		{
			tree.child_count(1);
			encode(*val, tree.child(0));
		}
		else
		{
			tree.child_count(0);
		}
	}

	template <typename Tree, typename T>
	void decode(const Tree &tree, std::optional<T> &val)
	{
		if (tree.child_count())
		{
			T inner;
			decode(tree.child(0), inner);
			val = std::move(inner);
		}
		else
		{
			val = std::nullopt;
		}
	}
}

#endif
