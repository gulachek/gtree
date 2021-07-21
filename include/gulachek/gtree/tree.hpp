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
	class is_tree
	{
		using Block = gulachek::gtree::block;

		// Assert that the tree has a value method:
		// const Block value() const;
		template <typename U>
		static constexpr bool has_value(...) { return false; }

		template <typename U,
		 decltype(std::declval<const U>().value())* ValuePtr = nullptr,
		 typename ValRet = std::remove_pointer_t<decltype(ValuePtr)>
							 >
		static constexpr typename std::enable_if<
			std::is_same<ValRet, const Block>::value
		,bool>::type has_value(void*) { return true; }

		// Assert that the tree has a child_count method:
		// std::size_t child_count() const;
		template <typename U>
		static constexpr bool has_child_count(...) { return false; }

		template <typename U,
		 decltype(std::declval<const U>().child_count())* CCPtr = nullptr,
		 typename Ret = std::remove_pointer_t<decltype(CCPtr)>
							 >
		static constexpr typename std::enable_if<
			std::is_same<Ret, std::size_t>::value
		,bool>::type has_child_count(void*) { return true; }

		// Assert that the tree has a child_count method:
		// is_tree<T> child(std::size_t i) const;
		template <typename U>
		static constexpr bool has_child(...) { return false; }

		template <typename TU, typename TRet>
		static constexpr std::enable_if_t<is_tree<TRet>::value, bool> second_is_tree;

		template <typename U,
		 std::remove_reference_t<decltype(std::declval<const U>().child(0))>* CPtr = nullptr,
		 typename RefRet = decltype(std::declval<const U>().child(0)),
		 typename Ret = std::remove_reference_t<RefRet>
							 >
		static constexpr typename std::enable_if<
			(std::is_const_v<Ret> ||
			!std::is_reference_v<RefRet>) &&
			std::is_same<Ret, U>::value
		,bool>::type has_child(void*) { return true; }

		template <typename U,
		 std::remove_reference_t<decltype(std::declval<const U>().child(0))>* CPtr = nullptr,
		 typename RefRet = decltype(std::declval<const U>().child(0)),
		 typename Ret = std::remove_reference_t<RefRet>
							 >
		static constexpr typename std::enable_if<
			(std::is_const_v<Ret> ||
			!std::is_reference_v<RefRet>) &&
			is_tree<Ret>::value
		,bool>::type has_child(void*) { return true; }

		static constexpr bool implies(bool p, bool q)
		{
			return (p && q) || !p;
		}

		public:
			static constexpr bool value =
				std::conjunction_v<
					std::is_default_constructible<T>,
					std::is_move_constructible<T>
						> &&
				has_value<T>(nullptr) &&
				has_child_count<T>(nullptr) &&
				has_child<T>(nullptr)
				;
	};

	template <typename T>
	constexpr bool is_tree_v = is_tree<T>::value;

	template <typename T>
	struct tree_implements_child_
	{
		constexpr static bool value = false;
	};

	template <typename T, typename Self>
	concept TreeImplementsChild_ =
		std::is_same_v<T, Self> || tree_implements_child_<T>::value;

	template <typename T>
	concept Tree = requires(const T t, std::size_t i)
	{
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

	template <typename Tree,
					 std::enable_if_t<is_tree<Tree>::value, int> = 0>
	bool is_empty(const Tree &t)
	{
		return t.value().empty() && t.child_count() == 0;
	}
}

#endif
