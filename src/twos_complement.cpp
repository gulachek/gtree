#include "gulachek/gtree/encoding/twos_complement.hpp"

namespace gulachek::gtree
{
	void twos_complement(void *buf, std::size_t n)
	{
		auto start = (std::uint8_t*)buf;

		std::uint8_t carry = 1;
		for (auto i = start; i < start + n; i++)
		{
			std::uint8_t b = *i;
			std::uint16_t sum = ~b + carry;
			carry = (sum & 0xff00);
			*i = (sum & 0xff);
		}
	}
}
