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
	cause read(std::istream &is, Decodable *target)
	{
		treeder reader{is};
		return reader.read(target);
	}

	template <decodable Decodable>
	cause read_fd(int fd, Decodable *target)
	{
		fd_istream_adapter adapt{fd};
		return read(adapt.stream(), target);
	}

	template <decodable Decodable>
	cause read_file(const std::filesystem::path &p, Decodable *target)
	{
		std::ifstream f{p};

		if (!f)
		{
			cause err;
			err << "error opening file " << p;
			return err;
		}

		return read(f, target);
	}
}

#endif
