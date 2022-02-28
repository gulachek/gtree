#ifndef GUlACHEK_GTREE_ENCODING_TWOS_COMPLEMENT_HPP
#define GUlACHEK_GTREE_ENCODING_TWOS_COMPLEMENT_HPP

#include <span>
#include <cstdint>

namespace gulachek::gtree
{
	void twos_complement(void *buf, std::size_t nbytes);
}

#endif
