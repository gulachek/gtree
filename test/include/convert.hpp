#ifndef TEST_CONVERT_HPP
#define TEST_CONVERT_HPP

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/read.hpp"

#include <sstream>

template <
	gulachek::gtree::encodable E,
	gulachek::gtree::decodable D>
gulachek::error cvt(const E &src, D *dest)
{
	std::stringstream ss;

	if (auto err = gulachek::gtree::write(ss, src))
		return err;

	ss.seekg(0, std::ios::beg);
	
	return gulachek::gtree::read(ss, dest);
}

#endif
