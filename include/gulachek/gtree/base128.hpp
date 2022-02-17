#ifndef GULACHEK_GTREE_BASE128_HPP
#define GULACHEK_GTREE_BASE128_HPP

#include <gulachek/cause.hpp>

#include <istream>
#include <cstddef>

namespace gulachek::gtree
{
	enum read_base128_error
	{
		incomplete = 1,
		bad_stream,
		overflow
	};

	cause read_base128(std::istream &is, std::size_t *n);
}

#endif
