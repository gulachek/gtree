#ifndef GULACHEK_GTREE_FD_HPP
#define GULACHEK_GTREE_FD_HPP

#include <iostream>
#include <memory>

namespace gulachek::gtree
{
	class fd_istream_adapter
	{
		public:
			fd_istream_adapter(int fd);
			~fd_istream_adapter();

			std::istream& stream();

		private:
			struct impl;
			std::unique_ptr<impl> pimpl_;
	};

	class fd_ostream_adapter
	{
		public:
			fd_ostream_adapter(int fd);
			~fd_ostream_adapter();

			std::ostream& stream();

		private:
			struct impl;
			std::unique_ptr<impl> pimpl_;
	};
}

#endif
