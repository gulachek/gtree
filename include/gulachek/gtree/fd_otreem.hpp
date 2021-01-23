#ifndef GULACHEK_GTREE_FD_OTREEM_HPP
#define GULACHEK_GTREE_FD_OTREEM_HPP

#include "gulachek/gtree/is_otreem.hpp"
#include "gulachek/gtree/otreem.hpp"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace gulachek::gtree
{
	namespace bio = boost::iostreams;

	class fd_otreem
	{
		public:
			fd_otreem(int fd) :
				_fdsink{fd, bio::never_close_handle},
				_sb{_fdsink},
				_os{&_sb},
				_ot{_os}
			{}

			template <typename Tree>
			void write(const Tree &t)
			{
				return _ot.write<Tree>(t);
			}

		private:
			bio::file_descriptor_sink _fdsink;
			bio::stream_buffer<bio::file_descriptor_sink> _sb;
			std::ostream _os;
			otreem _ot;
	};

	template <>
	struct is_otreem<fd_otreem> : std::true_type {};
}

#endif
