#ifndef STDIN_STREAM_HPP
#define STDIN_STREAM_HPP

#include <unistd.h>

#include <boost/asio.hpp>
#include <type_traits>

namespace gulachek::gtree::asio
{
	template <typename Executor = boost::asio::executor>
	class stdin_stream : public boost::asio::posix::basic_stream_descriptor<Executor>
	{
		public:
			typedef boost::asio::posix::basic_stream_descriptor<Executor> super;

			template<typename ExecutionContext>
				stdin_stream(ExecutionContext & context,
						std::enable_if_t<
						std::is_convertible<
							ExecutionContext &,
							boost::asio::execution_context &
							>::value
						>* = 0
						) : super{context, STDIN_FILENO} {}
	};
}

#endif
