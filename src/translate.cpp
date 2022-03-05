#include "gulachek/gtree/translate.hpp"

#include <boost/fiber/all.hpp>
#include <mutex>

namespace gulachek::gtree
{
	struct translate_stream::fiber
	{
		boost::fibers::mutex mtx;
		boost::fibers::condition_variable reading;
		boost::fibers::condition_variable writing;
	};

	translate_stream::translate_stream() :
		pfiber_{new translate_stream::fiber()}
	{}

	translate_stream::~translate_stream() {}

	void translate_stream::translate(
					const std::function<void()> &read,
					const std::function<void()> &write
			)
	{
		boost::fibers::fiber read_fib{read};
		boost::fibers::fiber write_fib{write};

		read_fib.join();
		done_reading();

		write_fib.join();
	}

	void translate_stream::done_reading()
	{
		is_reading_ = false;
		pfiber_->writing.notify_one();
	}

	void translate_stream::value(const void *data, std::size_t n)
	{
		auto start = (const std::uint8_t *)data;
		buf_ = std::vector<std::uint8_t>{start, start + n};
	}

	void translate_stream::child_count(std::size_t n)
	{
		child_count_ = n;

		std::unique_lock lck{pfiber_->mtx};

		is_reading_ = true;
		pfiber_->reading.notify_one();

		while (is_reading_)
			pfiber_->writing.wait(lck);
	}

	bool translate_stream::ok()
	{ return true; }

	cause translate_stream::next()
	{
		std::unique_lock lck{pfiber_->mtx};

		is_reading_ = false;
		pfiber_->writing.notify_one();

		while (!is_reading_)
			pfiber_->reading.wait(lck);

		return {};
	}

	std::size_t translate_stream::size() const
	{ return buf_.size(); }

	const std::uint8_t* translate_stream::data() const
	{ return buf_.data(); }

	std::size_t translate_stream::child_count() const
	{ return child_count_; }
}
