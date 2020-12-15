#include "gulachek/gtree/dynamic_tree.hpp"

namespace gulachek::gtree
{
	dynamic_tree::~dynamic_tree() {}

	const block dynamic_to_static::value() const
	{
		return _ptr->value();
	}

	std::size_t dynamic_to_static::child_count() const
	{
		return _ptr->child_count();
	}

	dynamic_to_static
		dynamic_to_static::child(std::size_t i) const
	{
		return dynamic_to_static{_ptr->child(i)};
	}
}
