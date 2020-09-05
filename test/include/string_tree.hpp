#ifndef STRING_TREE_HPP
#define STRING_TREE_HPP

#include "null_tree.hpp"

#include "gulachek/gtree.hpp"

#include <string>

class StringTree
{
	public:
		StringTree(const std::string &str = "") : _str{str} {}

		const gulachek::gtree::block value() const
		{ return {_str.data(), _str.size()}; }

		std::size_t child_count() const
		{ return 0; }

		NullTree child(std::size_t i) const
		{ return {}; }

	private:
		std::string _str;
};

#endif
