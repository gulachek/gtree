#ifndef GULACHEK_GTREE_CHILD_READER_HPP
#define GULACHEK_GTREE_CHILD_READER_HPP

#include "gulachek/gtree/base128.hpp"

#include <gulachek/cause.hpp>

#include <cstddef>
#include <istream>
#include <vector>

namespace gulachek::gtree
{
	template <typename T> struct decoding;

	enum class read_error
	{
		incomplete_value_size = 1,
		incomplete_value,
		incomplete_child_count,
		bad_children
	};

	class child_reader
	{
		public:
			child_reader(std::istream &is) :
				is_{is}
			{}

			template <typename Decodable>
			cause read(Decodable *target)
			{
				decoding<Decodable> dec{target};

				std::size_t nbytes;
				if (auto err = read_base128(is_, &nbytes))
				{
					if (err.is_eof())
						return err;

					cause wrap{read_error::incomplete_value_size};
					wrap << "error reading value size";
					wrap.add_cause(err);
					return wrap;
				}

				buf_.resize(nbytes);
				is_.read((char*)buf_.data(), nbytes);

				auto read_count = is_.gcount();
				if (read_count != nbytes)
				{
					cause err{read_error::incomplete_value};
					err << "reading tree value expected " << nbytes <<
						" bytes, actual: " << read_count;
					return err;
				}

				// invoke encoding specific logic
				if (auto err = dec.value(nbytes, buf_.data()))
				{
					cause wrap{"handling tree value"};
					wrap.add_cause(err);
					return wrap;
				}

				std::size_t nchildren;
				if (auto err = read_base128(is_, &nchildren))
				{
					cause wrap{read_error::incomplete_child_count};
					wrap << "error reading child count";
					wrap.add_cause(err);
					return wrap;
				}

				// handle each child
				if (auto err = dec.children(nchildren, *this))
				{
					cause wrap{read_error::bad_children, "handling tree children"};
					wrap.add_cause(err);
					return wrap;
				}

				return {};
			}

		private:
			std::istream &is_;
			std::vector<std::uint8_t> buf_;
	};

}

#endif
