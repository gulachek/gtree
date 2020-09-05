#include "gulachek/gtree/asio/stdin_stream.hpp"
#include "gulachek/gtree/asio/gnatz_reader.hpp"
#include <iostream>
#include <functional>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

template <typename NatNumReader>
class Session
{
	public:
		typedef NatNumReader reader_type;

		Session(reader_type &&reader) : _reader{std::move(reader)}
		{
			_do_read();
		}

	private:
		reader_type _reader;

		void _do_read()
		{
			using namespace std::placeholders;
			_reader.async_read(std::bind(&Session::_on_read, this, _1, _2));
		}

		void _on_read(const boost::system::error_code &ec, std::size_t &&n)
		{
			if (ec)
			{
				if (ec != asio::error::eof)
					std::cerr << ec.message() << std::endl;
				return;
			}

			std::cout << n << std::endl;
			_do_read();
		}
};

int main()
{
	asio::io_context io;
	gta::stdin_stream ain{io};
	asio::streambuf sb;
	gta::gnatz_reader gnz{ain, &sb};

	Session sess{std::move(gnz)};

	io.run();

	return 0;
}
