#ifndef GULACHEK_GTREE_GNATZ_WRITER_HPP
#define GULACHEK_GTREE_GNATZ_WRITER_HPP

#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <limits>

namespace gulachek::gtree
{
	template <typename StreamWriter>
	class gnatz_writer
	{
		public:
			gnatz_writer(StreamWriter &stream) :
				_stream{stream}
			{}

			std::size_t plan(std::size_t n) const
			{
				// special case
				if (n == 0) return 1;

				auto nbits =
					(n ==
					 std::numeric_limits<std::size_t>::max()) ? 
					sizeof(std::size_t) * 8 :
					std::log2(n + 1);

				return std::ceil(nbits / 7);
			}

			void write(std::size_t n)
			{
				while (n > 0x7f)
				{
					std::uint8_t b = (0x80 | (n & 0x7f));
					_stream.write((const char*)&b, 1);
					n >>= 7;
				}

				std::uint8_t b = n;
				_stream.write((const char*)&b, 1);
			}

		private:
			StreamWriter &_stream;
	};
}

#endif
