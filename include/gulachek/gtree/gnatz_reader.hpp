#ifndef GULACHEK_GTREE_GNATZ_READER_HPP
#define GULACHEK_GTREE_GNATZ_READER_HPP

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

namespace gulachek::gtree
{
	template <typename InStream>
	class gnatz_reader
	{
		public:
			gnatz_reader(InStream &stream) : _is{stream} {}

			bool read(std::size_t *n)
			{
				*n = 0;
				std::uint8_t b, offset = 0, count = 0;
				while (_is.read((char*) &b, 1))
				{
					count++;
					*n |= (((std::size_t) b & 0x7f) << offset);
					offset += 7;

					if (!(b & 0x80)) break;
				}

				if (_is.eof())
				{
					if (count)
						throw std::runtime_error{"Bad gnatz encoding"};
					else
						return false;
				}

				return true;
			}

		private:
			InStream &_is;
	};
}

#endif
