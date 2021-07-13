#ifndef GULACHEK_GTREE_READ_HPP
#define GULACHEK_GTREE_READ_HPP

#include "gulachek/gtree/error.hpp"
#include "gulachek/gtree/mutable_tree.hpp"

namespace gulachek::gtree
{
	template <typename TreeReader, typename T>
	error read(TreeReader &reader, T &t)
	{
		typename TreeReader::tree_type tr;

		if (!reader.read(&tr))
			return {"Failed to read tree"};

		if (auto err = decode(tr, t))
			return err;

		return {};
	}
}

#endif
