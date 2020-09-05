#ifndef GULACHEK_GTREE_ASIO_ERROR_HPP
#define GULACHEK_GTREE_ASIO_ERROR_HPP

#include <boost/asio.hpp>

namespace gulachek::gtree::asio::error
{
	using namespace boost::system;
#define DECL_ERR(CODE) constexpr error_code CODE = make_error_code(errc::CODE)
	DECL_ERR(bad_message);
	DECL_ERR(not_enough_memory);
#undef DECL_ERR
}

#endif
