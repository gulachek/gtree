#ifndef GULACHEK_GTREE_GNATZ_ITER_HPP
#define GULACHEK_GTREE_GNATZ_ITER_HPP

namespace gulachek::gtree
{
	// potentially make variable num type - right now commit to std::size_t
	class gnatz_iter
	{
		public:
			// could be more stl-y w/ iter templates, but this is what's needed right now
			// maybe wait for concepts to be a thing cuz that would be cool
			gnatz_iter(const std::uint8_t *data, std::size_t n) :
				_it{data}, _end{data + n}
			{}

			bool next(std::size_t *n)
			{
				if (_it >= _end)
					return false;

				*n = 0;
				std::uint8_t offset = 0, i = 0;
				std::uint8_t b;
				do
				{
					if (_it >= _end)
						throw std::runtime_error{"gnatz: expected terminating byte"};

					if (++i >= sizeof(std::size_t))
						throw std::overflow_error{"gnatz: number too big for std::size_t"};

					b = *_it;
					*n |= ((b & 0x7f) << offset);
					offset += 7;
					++_it;
				} while (b & 0x80);

				return true;
			}

		private:
			const std::uint8_t *_it;
			const std::uint8_t *_end;
	};
}

#endif
