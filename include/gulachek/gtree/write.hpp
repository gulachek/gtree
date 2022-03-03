#ifndef GULACHEK_GTREE_WRITE_HPP
#define GULACHEK_GTREE_WRITE_HPP

#include "gulachek/gtree/base128.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <gulachek/cause.hpp>

#include <ostream>
#include <span>
#include <cstddef>
#include <cstdint>

namespace gulachek::gtree
{
	template <encodable T>
	cause write(std::ostream &os, const T &val)
	{
		tree_writer writer{os};
		return writer.write(val);
	}
}

#endif
