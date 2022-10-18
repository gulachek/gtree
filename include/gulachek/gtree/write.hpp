#ifndef GULACHEK_GTREE_WRITE_HPP
#define GULACHEK_GTREE_WRITE_HPP

#include "gulachek/gtree/base128.hpp"
#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/fd.hpp"

#include <gulachek/error.hpp>

#include <ostream>
#include <span>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>

namespace gulachek::gtree
{
	template <encodable T>
	error write(std::ostream &os, const T &val)
	{
		ostream_tree_writer_stream stream{os};
		tree_writer writer{stream};
		return writer.write(val);
	}

	template <encodable T>
	error write_fd(int fd, const T &val)
	{
		fd_ostream_adapter adapt{fd};
		return write(adapt.stream(), val);
	}

	template <encodable T>
	error write_file(const std::filesystem::path &p, const T &val)
	{
		std::ofstream f{p};

		if (!f)
		{
			error err;
			err << "error opening file " << p;
			return err;
		}

		return write(f, val);
	}
}

#endif
