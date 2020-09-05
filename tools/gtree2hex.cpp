#include "gulachek/gtree/asio/stdin_stream.hpp"
#include "gulachek/gtree/asio/treeder.hpp"
#include <iostream>
#include <iomanip>
#include <functional>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;

template <typename TreeReader>
class Session
{
	public:
		typedef TreeReader reader_type;

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

		void _on_read(const boost::system::error_code &ec, gt::tree &&t)
		{
			if (ec)
			{
				if (ec != asio::error::eof)
					std::cerr << ec.message() << std::endl;
				return;
			}

			_print(t, 0);

			_do_read();
		}

		void _print(const gt::tree &t, std::size_t ident)
		{
			std::cout << std::string(ident, ' ') <<
					'<' << _hex(t.value()) << '>' << std::endl;

			for (auto i = 0; i < t.child_count(); i++)
				_print(t.child(i), ident+1);
		}

		static std::string _hex(const gt::block &b)
		{
			std::stringstream ss;
			ss << std::hex;
			for (auto i = 0; i < b.size(); i++)
				ss << std::setfill('0') << std::setw(2) << (int)b[i];

			return ss.str();
		}
};

int main()
{
	asio::io_context io;
	gt::asio::stdin_stream ain{io};
	gt::asio::treeder gt{ain};

	Session sess{std::move(gt)};

	io.run();

	return 0;
}
