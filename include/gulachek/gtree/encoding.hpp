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
		{ encoding<T>{val} };

		{enc.encode(writer)} -> my_same_as<cause>;
	};

	class tree_writer
	{
		enum class cursor_position
		{
			value,
			child_count,
			children
		};

		public:
			tree_writer(std::ostream &os) :
				os_{os},
				cursor_{cursor_position::children},
				nchildren_{~0LU},
				write_count_{0}
			{}

			void value(const void *buf, std::size_t nbytes)
			{
				if (cursor_ != cursor_position::value)
					throw std::logic_error{"Must write value immediately"};

				write_base128(os_, nbytes);
				os_.write((const char*)buf, nbytes);

				cursor_ = cursor_position::child_count;
			}

			void child_count(std::size_t n)
			{
				if (cursor_ != cursor_position::child_count)
					throw std::logic_error{"Must write child count immediately after value"};

				write_base128(os_, n);

				cursor_ = cursor_position::children;
				nchildren_ = n;
			}

			template <encodeable T>
			void write(const T &c)
			{
				if (cursor_ != cursor_position::children)
					throw std::logic_error{"Must write children immediately after child count"};

				if (++write_count_ > nchildren_)
					throw std::logic_error{"Wrote too many children"};

				encoding<T> enc{c};

				tree_writer writer{os_};
				writer.cursor_ = cursor_position::value;

				enc.encode(writer);

				if (writer.write_count_ < writer.nchildren_)
					throw std::logic_error{"Must write appropriate number of children"};
			}

		private:
			std::ostream &os_;
			cursor_position cursor_;
			std::size_t nchildren_;
			std::size_t write_count_;
	};

	template <typename T>
	concept class_encodable = requires
	(
	 const T &val,
	 tree_writer &writer
	 )
	{
		{ val.gtree_encode(writer) } -> my_same_as<cause>;
	};

	template <class_encodable T>
	struct encoding<T>
	{
		const T &val_;

		cause encode(tree_writer &writer)
		{
			return val_.gtree_encode(writer);
		}
	};
}

#endif
