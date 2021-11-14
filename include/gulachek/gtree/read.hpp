#ifndef GULACHEK_GTREE_READ_HPP
#define GULACHEK_GTREE_READ_HPP

#include "gulachek/gtree/error.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/itreem.hpp"

#include <fstream>
#include <filesystem>
#include <string_view>

namespace gulachek::gtree
{
	template <typename TreeReader, typename T>
		requires Tree<typename TreeReader::tree_type> // make a TreeReader concept later
	error read(TreeReader &reader, T &t)
	{
		typename TreeReader::tree_type tr;

		if (!reader.read(&tr))
			return {"Failed to read tree"};

		if (auto err = decode(tr, t))
			return err;

		return {};
	}

	template <typename T>
	error read(std::istream &is, T &t)
	{
		itreem treem{is};
		return read<itreem, T>(treem, t);
	}

	template <typename T>
	error read_file(const std::filesystem::path &path, T &t)
	{
		std::fstream f{path};
		if (!f)
		{
			error err;
			err << "Failed to open file: " << path;
			return err;
		}

		return read(f, t);
	}
}

#endif
