#include "gulachek/gtree/fd_itreem.hpp"

namespace gulachek::gtree
{
	fd_itreem::fd_itreem(int fd) :
		_fdsrc{fd, bio::never_close_handle},
		_sb{_fdsrc},
		_is{&_sb},
		_it{_is}
	{}

	fd_itreem::operator bool()
	{
		return (bool)_it;
	}

	bool fd_itreem::read(tree *ptree)
	{
		return _it.read(ptree);
	}
}
