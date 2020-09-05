#ifndef GULACHEK_GTREE_GNATZ_READER_HPP
#define GULACHEK_GTREE_GNATZ_READER_HPP

#include "gulachek/gtree/asio/error.hpp"
#include <boost/asio.hpp>

namespace gulachek::gtree::asio
{
	template <
		typename AsyncReadStream,
		typename NatNum = std::size_t
		>
	class gnatz_reader
	{
		public:
			typedef typename AsyncReadStream::executor_type executor_type; 

			using Handler = void(const boost::system::error_code &, NatNum&&);

			gnatz_reader(AsyncReadStream &stream, boost::asio::streambuf *pbuf) :
				_stream{stream},
				_buffer{pbuf}
			{}

			executor_type get_executor()
			{
				return _stream.get_executor();
			}

			template <typename CompletionToken>
			auto async_read(CompletionToken &&token)
			{
				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, Handler>::completion_handler_type
					handler{std::forward<CompletionToken>(token)};

				auto f = [this, handler]
					(const boost::system::error_code &ec, std::size_t nread) mutable
					{
						if (ec == boost::asio::error::eof)
						{
							handler(
									_buffer->size() ? error::bad_message : boost::asio::error::eof,
									0
									);
							return;
						}

						auto ec_copy = ec;
						auto length = nread + 1;
						auto begin = boost::asio::buffers_begin(_buffer->data());
						auto n = parse_num(begin, begin + length, ec_copy);
						_buffer->consume(length);
						handler(ec_copy, std::move(n));
					};

				auto ex2 = boost::asio::get_associated_executor(
						handler,
						get_executor()
						);

				boost::asio::async_read_until(
						_stream, 
						*_buffer,
						match_high_bit_unset,
						boost::asio::bind_executor(ex2, std::move(f))
						);

				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, Handler> ret{handler};
				return ret.get();
			}

		private:
			AsyncReadStream &_stream;
			boost::asio::streambuf *_buffer;

			typedef boost::asio::buffers_iterator<
				boost::asio::streambuf::const_buffers_type> iterator;
			static std::pair<iterator, bool> match_high_bit_unset(
					iterator begin, iterator end)
			{
				for (auto i = begin; i != end; i++)
				{
					if (!(*i & 0x80))
					{
						return {i, true};
					}
				}
				return {end, false};
			}

			template <typename Iterator>
			NatNum parse_num(Iterator begin, Iterator end, boost::system::error_code &ec)
			{

				NatNum val = 0, offset = 0, mask = 0x7f;
				for (auto b = begin; b != end; b++, offset += 7)
					val |= ((*b & mask) << offset);
				return val;
			}
	};

}

#endif
