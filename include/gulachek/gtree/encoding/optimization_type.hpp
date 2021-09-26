#ifndef GULACHEK_GTREE_ENCODING_OPTIMIZATION_TYPE
#define GULACHEK_GTREE_ENCODING_OPTIMIZATION_TYPE

#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/enum.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include <cstdint>
#include <ostream>

namespace gulachek::gtree
{
	enum class optimization_type : std::uint8_t
	{
		empty = 0, // no value, no children
		value = 1, // uses_value, no children
		container = 2, // no value, uses_children
		hybrid = 3 // uses_value, uses_children
	};

	// not sure why you'd need 'empty' for encoding optimization but seems incomplete w/o

	std::ostream& operator << (std::ostream &os, optimization_type ot);
}

#endif
