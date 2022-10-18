#ifndef GULACHEK_GTREE_READ_HPP
#define GULACHEK_GTREE_READ_HPP

#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/fd.hpp"

#include <istream>
#include <type_traits>

#include <vector>
#include <sstream>
#include <string>
#include <span>
#include <cstdint>
#include <ostream>
#include <filesystem>
#include <fstream>

namespace gulachek::gtree
{
	template <decodable Decodable>
	error read(std::istream &is, Decodable *target)
	{
		istream_treeder_stream stream{is};
		treeder reader{stream};
		return reader.read(target);
	}

	template <decodable Decodable>
	error read_fd(int fd, Decodable *target)
	{
		fd_istream_adapter adapt{fd};
		if (auto err = read(adapt.stream(), target))
		{
			if (err.is_eof())
				return err;

			return err.wrap() << "error reading fd " << fd;
		}

		return {};
	}

	template <decodable Decodable>
	error read_file(const std::filesystem::path &p, Decodable *target)
	{
		std::ifstream f{p};

		if (!f)
		{
			error err;
			err << "error opening file " << p;
			return err;
		}

		if (auto err = read(f, target))
		{
			if (err.is_eof())
				return err;

			return err.wrap() << "error reading file " << p;
		}

		return {};
	}
}

#endif
