#ifndef GULACHEK_GTREE_WRITE_HPP
#define GULACHEK_GTREE_WRITE_HPP

#include "gulachek/gtree/error.hpp"
#include "gulachek/gtree/mutable_tree.hpp"

namespace gulachek::gtree
{
	template <typename TreeWriter, typename T>
	auto write(TreeWriter &writer, T &&t)
	{
		mutable_tree tr;
		auto err = encode(std::forward<T>(t), tr);
		writer.write(tr);
		return err;
	}
}

#endif
