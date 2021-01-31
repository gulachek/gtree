#ifndef GULACHEK_GTREE_DYNAMIC_TREE_HPP
#define GULACHEK_GTREE_DYNAMIC_TREE_HPP

#include "gulachek/gtree/tree.hpp"
#include <memory>

namespace gulachek::gtree
{
	// Some use cases need runtime computation (like plugins)
	struct dynamic_tree
	{
		virtual ~dynamic_tree();

		virtual const block value() const = 0;
		virtual std::size_t child_count() const = 0;
		virtual std::shared_ptr<const dynamic_tree>
			child(std::size_t i) const = 0;
	};

	typedef std::shared_ptr<const dynamic_tree> dynamic_ref;

	template <typename T, typename ...Args>
	std::shared_ptr<T> make_dynamic(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	// Implements static tree requirements
	template <typename DynamicPtr>
	class dynamic_to_static
	{
		public:
			dynamic_to_static() : _ptr{nullptr} {}

			dynamic_to_static(DynamicPtr tr) : _ptr{tr}
			{}

			const block value() const
			{
				return _ptr->value();
			}

			std::size_t child_count() const
			{
				return _ptr->child_count();
			}

			auto child(std::size_t i) const
			{
				return dynamic_to_static<dynamic_ref>
					{_ptr->child(i)};
			}

		private:
			DynamicPtr _ptr;
	};

	// Adapt static tree to dynamic
	template <typename Tree>
	class static_to_dynamic : public dynamic_tree
	{
			typedef static_to_dynamic<Tree> tree_type;

		public:
			static_to_dynamic(const Tree &t) : _t{t} {}

			const block value() const override
			{
				return _t.value();
			}

			std::size_t child_count() const override
			{
				return _t.child_count();
			}

			std::shared_ptr<const dynamic_tree>
				child(std::size_t i) const override
			{
				return std::make_shared<static_to_dynamic>(
						_t.child(i)
						);
			}

		private:
			Tree _t;
	};

	template <typename Tree>
	dynamic_ref to_dynamic(const Tree &t)
	{
		return make_dynamic<static_to_dynamic<Tree>>(t);
	}
}

#endif
