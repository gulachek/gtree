#ifndef COPY_TREE_HPP
#define COPY_TREE_HPP

#include "gulachek/gtree.hpp"
#include <vector>
#include <cstdint>

class CopyTree
{
	public:
		CopyTree() = default;

		template <typename Tree>
		CopyTree(const Tree &tree)
		{
			auto val = tree.value();
			_value = std::vector<std::uint8_t>{val.data(), val.data() + val.size()};

			_children.reserve(tree.child_count());
			for (std::size_t i = 0; i < tree.child_count(); i++)
				_children.emplace_back(CopyTree{tree.child(i)});
		}

		const gulachek::gtree::block value() const
		{ return {_value.data(), _value.size()}; }

		std::size_t child_count() const
		{ return _children.size(); }

		CopyTree child(std::size_t i) const
		{ return _children[i]; }

	private:
		std::vector<std::uint8_t> _value;
		std::vector<CopyTree> _children;
};

#endif
