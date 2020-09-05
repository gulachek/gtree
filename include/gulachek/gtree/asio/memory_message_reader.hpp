#ifndef GULACHEK_GTREE_ASIO_MEMORY_MESSAGE_READER_HPP
#define GULACHEK_GTREE_ASIO_MEMORY_MESSAGE_READER_HPP

#include <boost/asio.hpp>
#include <queue>
#include <functional>
#include <mutex>
#include <memory>

namespace gulachek::gtree::asio
{
	template <
		typename Message,
		typename Executor = boost::asio::executor
		>
	class memory_message_reader
	{
		public:
			typedef Message message_type;
			typedef Executor executor_type;

			template <typename ExecutionContext>
			explicit memory_message_reader(
				ExecutionContext& context,
				typename std::enable_if<
					std::is_convertible<
						ExecutionContext&,
						boost::asio::execution_context&
					>::value,
				memory_message_reader>::type* = 0)
			:
				_executor{context.get_executor()},
				_plock{std::make_unique<std::mutex>()}
			{}

			executor_type get_executor()
			{
				return _executor;
			}

			void write(const message_type &msg)
			{
				std::lock_guard lock{*_plock};
				_ops.push({{}, msg});
				_notify();
			}

			void error(const boost::system::error_code &ec)
			{
				std::lock_guard lock{*_plock};
				_ops.push({ec, {}});
				_notify();
			}

			template <typename CompletionToken>
			auto async_read(CompletionToken &&token)
			{
				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, handler_type>::completion_handler_type
					handler{std::forward<CompletionToken>(token)};

				typename boost::asio::async_result<
					std::decay_t<CompletionToken>, handler_type> ret{handler};

				auto ex2 = _handler_executor(handler);

				std::lock_guard lock{*_plock};
				if (_ops.empty())
				{
					_queue_read_op(std::move(handler));
				}
				else
				{
					ex2.post(_resolve(handler), std::allocator<void>{});
				}

				return ret.get();
			}

		private:
			typedef void handler_type(const boost::system::error_code &, message_type &&);

			struct Operation
			{
				boost::system::error_code ec;
				message_type msg;
			};

			executor_type _executor;
			std::queue<std::function<void(void)>> _work;
			std::queue<Operation> _ops;
			std::unique_ptr<std::mutex> _plock;

			template <typename CompletionHandler>
			auto _resolve(CompletionHandler handler)
			{
				auto f = [handler, op{std::move(_ops.front())}]() mutable {
					handler(op.ec, std::move(op.msg));
				};
				_ops.pop();
				return f;
			}

			template <typename CompletionHandler>
			auto _handler_executor(const CompletionHandler &handler)
			{
				return boost::asio::get_associated_executor(handler, get_executor());
			}

			template <typename CompletionHandler>
			void _queue_read_op(CompletionHandler &&handler)
			{
					auto ex2 = _handler_executor(handler);
					auto work1 = boost::asio::make_work_guard(get_executor());
					auto work2 = boost::asio::make_work_guard(ex2);

					auto completion =
					[this, handler,
						w1{std::move(work1)}, w2{std::move(work2)},
						ex2] () mutable
					{
						ex2.dispatch(_resolve(handler), std::allocator<void>{});
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
	};

}

#endif
