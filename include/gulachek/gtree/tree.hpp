#ifndef GULACHEK_GTREE_TREE_HPP
#define GULACHEK_GTREE_TREE_HPP

#include "gulachek/gtree/block.hpp"
#include "gulachek/gtree/tree_base.hpp"
#include <memory>
#include <type_traits>
#include <concepts>

namespace gulachek::gtree
{
	class tree
	{
		public:
			// represents empty-value root w/ no children
			tree();

			tree(
					const std::shared_ptr<tree_base> &base,
					std::size_t index
					);

			// Access value of tree's root node
			const block value() const
			{ return _node.value; }

			// How many children?
			std::size_t child_count() const
			{ return _node.child_count; }

			// Access children
			tree child(std::size_t index) const;

		private:
			// Fundamental structure
			std::shared_ptr<tree_base> _base;

			// Cache to avoid going through pointer
			node _node;
	};

	template <typename T>
	struct tree_implements_child_
	{
		constexpr static bool value = false;
	};

	template <typename T, typename Self>
	concept TreeImplementsChild_ =
		std::is_same_v<std::decay_t<T>, Self> ||
		tree_implements_child_<std::decay_t<T>>::value;

	template <typename T>
	concept Tree = requires(const T t, std::size_t i)
	{
		requires std::is_default_constructible_v<std::decay_t<T>>;
		requires std::is_move_constructible_v<std::decay_t<T>>;
		{ t.value() } -> std::same_as<const block>;
		{ t.child_count() } -> std::same_as<std::size_t>;
		{ t.child(i) } -> TreeImplementsChild_<T>;
	};

	template <Tree T>
	struct tree_implements_child_<T>
	{
		constexpr static bool value = true;
	};

	template <Tree TLeft, Tree TRight>
	bool operator == (const TLeft &left, const TRight &right)
	{
		auto lcnt = left.child_count(), rcnt = right.child_count();

		if (lcnt != rcnt)
			return false;

		if (left.value() != right.value())
			return false;

		for (std::size_t i = 0; i < lcnt; i++)
			if (left.child(i) != right.child(i))
				return false;

		return true;
	}

	template <Tree TLeft, Tree TRight>
	bool operator != (const TLeft &left, const TRight &right)
	{
		return !(left == right);
	}

	template <Tree Tr>
	bool is_empty(const Tr &t)
	{
		return t.value().empty() && t.child_count() == 0;
	}
}

#endif
