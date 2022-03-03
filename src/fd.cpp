#include "gulachek/gtree/fd.hpp"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace gulachek::gtree
{
	namespace bio = boost::iostreams;

	class fd_istream_adapter::impl
	{
		public:
			impl(int fd) :
				fdsrc_{fd, bio::never_close_handle},
				sb_{fdsrc_},
				is_{&sb_}
			{}

			std::istream& stream() { return is_; }

		private:
			bio::file_descriptor_source fdsrc_;
			bio::stream_buffer<bio::file_descriptor_source> sb_;
			std::istream is_;
	};

	fd_istream_adapter::fd_istream_adapter(int fd) :
		pimpl_{std::make_unique<fd_istream_adapter::impl>(fd)}
	{}

	fd_istream_adapter::~fd_istream_adapter() {}

	std::istream& fd_istream_adapter::stream()
	{ return pimpl_->stream(); }

	class fd_ostream_adapter::impl
	{
		public:
			impl(int fd) :
				fdsink_{fd, bio::never_close_handle},
				sb_{fdsink_},
				os_{&sb_}
			{}

			std::ostream& stream() { return os_; }

		private:
			bio::file_descriptor_sink fdsink_;
			bio::stream_buffer<bio::file_descriptor_sink> sb_;
			std::ostream os_;
	};

	fd_ostream_adapter::fd_ostream_adapter(int fd) :
		pimpl_{std::make_unique<fd_ostream_adapter::impl>(fd)}
	{}

	fd_ostream_adapter::~fd_ostream_adapter() {}

	std::ostream& fd_ostream_adapter::stream()
	{ return pimpl_->stream(); }
}
