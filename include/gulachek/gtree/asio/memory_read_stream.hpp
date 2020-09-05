#ifndef GULACHEK_GTREE_ASIO_MEMORY_READ_STREAM_HPP
#define GULACHEK_GTREE_ASIO_MEMORY_READ_STREAM_HPP

#include <boost/asio.hpp>
#include <type_traits>
#include <memory>
#include <functional>
#include <queue>
#include <list>
#include <iostream>

namespace gulachek::gtree::asio
{
	template <typename Executor = boost::asio::executor>
	class memory_read_stream
	{
		public:
			typedef Executor executor_type;
			typedef void ReadHandler(const boost::system::error_code &ec, std::size_t);

			executor_type get_executor()
			{
				return _executor;
			}

			memory_read_stream(memory_read_stream &&) = default;
			memory_read_stream(const memory_read_stream &) = delete;

			template <typename ExecutionContext>
			explicit memory_read_stream(
					ExecutionContext& context,
				typename std::enable_if<
				std::is_convertible<
					ExecutionContext&,
					boost::asio::execution_context&
					>::value,
				memory_read_stream>::type* = 0)
			:
				_executor{context.get_executor()}
			{
			}

			template <typename MutableBufferSequence, typename CompletionToken>
			auto async_read_some(
					const MutableBufferSequence &buffers,
					CompletionToken&& token
					)
			{
				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, ReadHandler>::completion_handler_type
					handler{std::forward<CompletionToken>(token)};

				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, ReadHandler> ret{handler};

				auto ex2 = _handler_executor(handler);

				// Return immediately if buffer to read into is empty
				if (boost::asio::buffer_size(buffers) == 0)
				{
					auto f = std::bind(handler, boost::system::error_code{}, 0);
					ex2.post(std::move(f), std::allocator<void>{});
					return ret.get();
				}

				if (_ops.empty())
				{
					_queue_read_op(buffers, std::move(handler));
				}
				else
				{
					ex2.post(_resolve(buffers, handler), std::allocator<void>{});
				}

				return ret.get();
			}

			template <typename ConstBufferSequence>
			std::size_t write_some(const ConstBufferSequence &buffers,
					boost::system::error_code &ec)
			{
				ec = {};

				std::string base;
				base.resize(boost::asio::buffer_size(buffers));

				auto n = boost::asio::buffer_copy(
						boost::asio::buffer(base), buffers);
				_ops.push_back({{}, std::move(base)});

				if (n) _notify();
				return n;
			}

			template <typename ConstBufferSequence>
			std::size_t write_some(const ConstBufferSequence &buffers)
			{
				boost::system::error_code ec;
				auto n = write_some(buffers, ec);
				boost::asio::detail::throw_error(ec, "write_some");
				return n;
			}

			void error(const boost::system::error_code &ec)
			{
				_ops.push_back({ec, {}});
				_notify();
			}

		private:
			struct Op
			{
				boost::system::error_code ec;
				std::string data;
			};

			template <typename MutableBufferSequence, typename CompletionHandler>
			auto _resolve(
					const MutableBufferSequence &buffers,
					const CompletionHandler &handler
					)
			{
					auto &op = _ops.front();

					auto n = boost::asio::buffer_copy(
							buffers, boost::asio::buffer(op.data));

					if (n < op.data.size())
					{
						op.data = op.data.substr(n);
					}
					else
					{
						_ops.pop_front();
					}

					return std::bind(handler, op.ec, n);
			}

			template <typename CompletionHandler>
			auto _handler_executor(const CompletionHandler &handler)
			{
				return boost::asio::get_associated_executor(handler, get_executor());
			}

			template <typename MutableBufferSequence, typename CompletionHandler>
			void _queue_read_op(
					const MutableBufferSequence &buffers,
					CompletionHandler &&handler
					)
			{
					auto ex2 = _handler_executor(handler);
					auto work1 = boost::asio::make_work_guard(get_executor());
					auto work2 = boost::asio::make_work_guard(ex2);

					auto completion =
					[this, buffers, handler,
						w1{std::move(work1)}, w2{std::move(work2)},
						ex2] () mutable
					{
						ex2.dispatch(_resolve(buffers, handler), std::allocator<void>{});
					};

					_work.emplace(std::move(completion));
			}
		
			void _notify()
			{
				if (!_work.empty())
				{
					auto work = std::move(_work.front());
					_work.pop();
					work();
				}
			}

			std::list<Op> _ops;
			std::queue<std::function<void(void)>> _work;
			executor_type _executor;
	};

}

#endif
