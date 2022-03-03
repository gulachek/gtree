#ifndef GULACHEK_GTREE_WRITE_HPP
#define GULACHEK_GTREE_WRITE_HPP

#include "gulachek/gtree/base128.hpp"
#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/fd.hpp"

#include <gulachek/cause.hpp>

#include <ostream>
#include <span>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace gulachek::gtree
{
	template <encodable T>
	cause write(std::ostream &os, const T &val)
	{
		tree_writer writer{os};
		return writer.write(val);
	}

	template <encodable T>
	cause write_fd(int fd, const T &val)
	{
		fd_ostream_adapter adapt{fd};
		return write(adapt.stream(), val);
	}

	template <encodable T>
	cause write_file(const std::filesystem::path &p, const T &val)
	{
		std::ofstream f{p};

		if (!f)
		{
			cause err;
			err << "error opening file " << p;
			return err;
		}

		return write(f, val);
	}
}

#endif
