#ifndef GULACHEK_GTREE_READ_HPP
#define GULACHEK_GTREE_READ_HPP

#include "gulachek/gtree/decoding.hpp"

#include <istream>
#include <type_traits>

#include <vector>
#include <sstream>
#include <string>
#include <span>
#include <cstdint>
#include <ostream>

namespace gulachek::gtree
{
	template <decodable Decodable>
	cause read(std::istream &is, Decodable *target)
	{
		treeder reader{is};
		return reader.read(target);
	}
}

#endif
