#ifndef GULACHEK_GTREE_BASE128_HPP
#define GULACHEK_GTREE_BASE128_HPP

#include <gulachek/cause.hpp>

#include <istream>
#include <ostream>
#include <cstddef>

namespace gulachek::gtree
{
	enum class read_base128_error
	{
		incomplete = 1,
		bad_stream,
		overflow
	};

	GTREE_API cause read_base128(std::istream &is, std::size_t *n);

	enum class write_base128_error
	{
		bad_stream = 1
	};

	GTREE_API cause write_base128(std::ostream &os, std::size_t n);
}

#endif
