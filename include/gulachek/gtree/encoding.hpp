#ifndef GULACHEK_GTREE_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_HPP

#include "gulachek/gtree/my_same_as.hpp"
#include "gulachek/gtree/base128.hpp"

#include <gulachek/cause.hpp>

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename T>
	struct encoding {};

	class tree_writer;

	template <typename T>
	concept encodeable = requires
	(
	 const T &val,
	 encoding<T> enc,
	 tree_writer &writer
	)
	{
		typename encoding<T>;
		requires std::is_constructible_v<encoding<T>, const T &>;

		{enc.encode(writer)} -> my_same_as<cause>;
	};

	class tree_writer
	{
		public:
			tree_writer(std::ostream &os) :
				os_{os}
			{}

			void value(const void *buf, std::size_t nbytes)
			{
				write_base128(os_, nbytes);
				os_.write((const char*)buf, nbytes);
			}

			void child_count(std::size_t n)
			{
				write_base128(os_, n);
			}

			template <encodeable T>
			void write(const T &c)
			{
				encoding<T> enc{c};
				enc.encode(*this);
			}

		private:
			std::ostream &os_;
	};
}

#endif
