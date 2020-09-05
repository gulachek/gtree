#ifndef GULACHEK_GTREE_ASIO_GTREE_READER_HPP
#define GULACHEK_GTREE_ASIO_GTREE_READER_HPP

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/asio/error.hpp"
#include <boost/asio.hpp>
#include <vector>
#include <functional>
#include <algorithm>

namespace gulachek::gtree::asio
{
	template <
		typename NatNumReader,
		typename AsyncReadStream,
		typename TreeBuilder
		>
	class gtree_reader
	{
		public:
			typedef typename NatNumReader::executor_type executor_type;

			gtree_reader(
					NatNumReader &nums,
					AsyncReadStream &stream,
					TreeBuilder &builder,
					boost::asio::streambuf &buf
					) :
				_size_reader{nums},
				_stream{stream},
				_builder{builder},
				_buffer{buf},
				_dispatch{nullptr}
			{}

			executor_type get_executor()
			{
				return _size_reader.get_executor();
			}

			template <typename CompletionToken>
			auto async_read(CompletionToken &&token)
			{
				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, handler_type>::completion_handler_type
					handler{std::forward<CompletionToken>(token)};

				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, handler_type> ret{handler};

				_dispatch = [this, task = Task<decltype(handler)>{handler}]
					(const boost::system::error_code &ec, tree &&tree)
					mutable
					{
						auto f = [ec, tr = std::move(tree), h = task.handler]() mutable {
							h(ec, std::move(tr));
						};

						auto ex2 = _handler_executor(task.handler);
						ex2.dispatch(std::move(f), std::allocator<void>{});
					};

				_read_size();

				return ret.get();
			}

		private:
			typedef void handler_type(const boost::system::error_code &ec, 
					tree &&tree);

			template <typename CompletionHandler>
			struct Task
			{
				CompletionHandler handler;
			};

			NatNumReader &_size_reader;
			AsyncReadStream &_stream;
			TreeBuilder &_builder;
			boost::asio::streambuf &_buffer;
			std::function<handler_type> _dispatch;

			template <typename CompletionHandler>
			auto _handler_executor(const CompletionHandler &handler)
			{
				return boost::asio::get_associated_executor(handler, get_executor());
			}

			template <typename CompletionHandler, typename Callable>
			auto _bind_executor(CompletionHandler handler, Callable &&callable)
			{
				return boost::asio::bind_executor(
						_handler_executor(handler),
						callable
						);
			}

			void _read_size()
			{
				_size_reader.async_read([this]
					(const boost::system::error_code &ec, std::size_t node_block_size)
					{
						if (ec || node_block_size == 0)
						{
							_dispatch(ec, tree{});
						}
						else
						{
							_read_node_block(node_block_size);
						}
					});
			}

			void _read_node_block(std::size_t block_size)
			{
				auto num_to_read = block_size - std::min(_buffer.size(), block_size);

				boost::asio::async_read(_stream, _buffer,
					boost::asio::transfer_exactly(num_to_read),
					[this, block_size]
					(const boost::system::error_code &ec, std::size_t nread)
					{
						if (ec)
						{
							_dispatch(
									ec == boost::asio::error::eof ? error::bad_message : ec,
									tree{}
									);
						}
						else
						{
							try
							{
								auto seq = _buffer.data();
								std::vector<std::uint8_t> cpy{
									buffers_begin(seq),
									buffers_begin(seq) + block_size
									};
								_buffer.consume(block_size);

								_read_data_block(
										_builder.node_block({cpy.data(), block_size})
										);
							}
							catch (const std::exception &ex)
							{
								_dispatch(error::bad_message, tree{});
							}
						}
					});
			}

			void _read_data_block(block &&data_block)
			{
				auto data_block_buf = boost::asio::buffer(data_block.data(), data_block.size());
				auto ncopied = boost::asio::buffer_copy(data_block_buf, _buffer.data());
				_buffer.consume(ncopied);

				if (ncopied == data_block.size())
				{
					_complete_data_block();
					return;
				}

				auto remaining = data_block_buf + ncopied;

				boost::asio::async_read(
					_stream,
					remaining,
					boost::asio::transfer_exactly(remaining.size()),
					[this]
					(const boost::system::error_code &ec, std::size_t nread)
					{
						if (ec)
						{
							_dispatch(
									ec == boost::asio::error::eof ? error::bad_message : ec,
									tree{}
									);
						}
						else
						{
							_complete_data_block();
						}
					});
			}

			void _complete_data_block()
			{
				try
				{
					_dispatch(boost::system::error_code{}, _builder.data_block());
				}
				catch (const std::exception &ex)
				{
					_dispatch(error::bad_message, tree{});
				}
			}
	};

}

#endif
