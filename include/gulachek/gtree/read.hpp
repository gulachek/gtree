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
		istream_treeder_stream stream{is};
		treeder reader{stream};
		return reader.read(target);
	}

	template <decodable Decodable>
	cause read_fd(int fd, Decodable *target)
	{
		fd_istream_adapter adapt{fd};
		if (auto err = read(adapt.stream(), target))
		{
			if (err.is_eof())
				return err;

			cause wrap;
			wrap << "error reading fd " << fd;
			wrap.add_cause(std::move(err));
			return wrap;
		}

		return {};
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

		if (auto err = read(f, target))
		{
			if (err.is_eof())
				return err;

			cause wrap;
			wrap << "error reading file " << p;
			wrap.add_cause(std::move(err));
			return wrap;
		}

		return {};
	}
}

#endif
