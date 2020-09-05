#ifndef GULACHEK_GTREE_BLOCK_HPP
#define GULACHEK_GTREE_BLOCK_HPP

#include <cstdlib>
#include <cstdint>
#include <ostream>
#include <string>

namespace gulachek::gtree
{
	class block
	{
		public:
			// Default construct an empty block
			block() :
				_data{nullptr},
				_size{0}
				{}

			template <typename T>
			block(T *t, std::size_t n) :
				_data{(std::uint8_t*) t},
				_size{n * sizeof(T)}
			{}

			block(decltype(nullptr) _, std::size_t n) :
				_data{nullptr},
				_size{n}
			{}

			// Pointer to block of data
			const std::uint8_t* data() const
			{ return _data; }

			std::uint8_t* data()
			{ return _data; }

			// Number of bytes in block
			const std::size_t size() const
			{ return _size; }

			std::uint8_t operator[] (std::size_t i) const
			{ return _data[i]; }

			std::uint8_t& operator[] (std::size_t i)
			{ return _data[i]; }

			bool empty() const { return !_size; }
			operator bool () { return _size; }

		protected:
			std::uint8_t *_data;
			std::size_t _size;
	};

}

bool operator == (
		const gulachek::gtree::block &left,
		const gulachek::gtree::block &right
		);

bool operator != (
		const gulachek::gtree::block &left,
		const gulachek::gtree::block &right
		);

namespace std
{
	std::string to_string(
			const gulachek::gtree::block &block);
}

std::ostream& operator << (std::ostream& os,
		const gulachek::gtree::block &b);

#endif
