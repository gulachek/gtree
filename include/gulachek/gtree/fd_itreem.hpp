#ifndef GULACHEK_GTREE_FD_ITREEM_HPP
#define GULACHEK_GTREE_FD_ITREEM_HPP

#include "gulachek/gtree/is_itreem.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/itreem.hpp"

#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

namespace gulachek::gtree
{
	namespace bio = boost::iostreams;

	class fd_itreem
	{
		public:
			typedef tree tree_type;

			fd_itreem(int fd);
			operator bool();
			bool read(tree *ptree);

		private:
			bio::file_descriptor_source _fdsrc;
			bio::stream_buffer<bio::file_descriptor_source> _sb;
			std::istream _is;
			itreem _it;
	};

	template <>
	struct is_itreem<fd_itreem> : std::true_type {};
}

#endif
