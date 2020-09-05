#define BOOST_TEST_MODULE MemoryReadStreamTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/asio/memory_read_stream.hpp"
#include <sstream>
#include <utility>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(MoveConstructible)
{
	bool is_move_constructible =
		std::is_move_constructible<gta::memory_read_stream<>>::value;
	BOOST_TEST(is_move_constructible);
}

BOOST_AUTO_TEST_CASE(CopyDeleted)
{
	bool is_copy_constructible =
		std::is_copy_constructible<gta::memory_read_stream<>>::value;
	BOOST_TEST(!is_copy_constructible);
}

BOOST_AUTO_TEST_CASE(ExecutorTypeDefaultsToAsioExecutorType)
{
	bool is_executor =
		std::is_same<gta::memory_read_stream<>::executor_type,
			asio::executor
				>::value;
	BOOST_TEST(is_executor);
}

BOOST_AUTO_TEST_SUITE_END() // TypeTraits

BOOST_AUTO_TEST_SUITE(GetExecutor)

BOOST_AUTO_TEST_CASE(ConstructWithIoContextHasSameGetExecutor)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	BOOST_CHECK(stream.get_executor() == io.get_executor());
}

BOOST_AUTO_TEST_SUITE_END() // GetExecutor

BOOST_AUTO_TEST_SUITE(Writing)

BOOST_AUTO_TEST_CASE(WriteSomePutsWholeBufferInQueue)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	auto n = stream.write_some(asio::buffer(msg));

	BOOST_TEST(n == msg.size());
}

BOOST_AUTO_TEST_CASE(AsioWritePutsWholeBuffer)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	auto n = asio::write(stream, asio::buffer(msg), asio::transfer_all());

	BOOST_TEST(n == msg.size());
}

BOOST_AUTO_TEST_SUITE_END() // Writing

BOOST_AUTO_TEST_SUITE(Reading)

BOOST_AUTO_TEST_SUITE(ReadSomeTakesEntireWriteSome)

BOOST_AUTO_TEST_CASE(Sync)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	stream.write_some(asio::buffer(msg));

	std::string msg_read;
	msg_read.resize(msg.size());

	auto nread = stream.async_read_some(
			asio::buffer(msg_read),
			asio::use_future
			);

	io.run();

	BOOST_TEST(nread.get() == msg.size());
	BOOST_TEST(msg_read == msg);
}

BOOST_AUTO_TEST_CASE(Async)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string msg_read;
	msg_read.resize(msg.size());

	auto nread = stream.async_read_some(
			asio::buffer(msg_read),
			asio::use_future
			);

	stream.write_some(asio::buffer(msg));

	io.run();

	BOOST_TEST(nread.get() == msg.size());
	BOOST_TEST(msg_read == msg);
}

BOOST_AUTO_TEST_SUITE_END() // ReadSomeTakesEntireWriteSome

BOOST_AUTO_TEST_SUITE(SequentialWritesResultInSequentialReads)

BOOST_AUTO_TEST_CASE(Sync)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	stream.write_some(asio::buffer(msg));
	stream.write_some(asio::buffer(msg));

	std::string first, second;
	first.resize(2 * msg.size());
	second.resize(2 * msg.size());

	auto nfirst = stream.async_read_some(
			asio::buffer(first),
			asio::use_future
			);

	io.run();

	first.resize(nfirst.get());
	BOOST_TEST(first == msg);

	io.restart();

	auto nsecond = stream.async_read_some(
			asio::buffer(second),
			asio::use_future
			);

	io.run();

	second.resize(nsecond.get());
	BOOST_TEST(second == msg);
}

BOOST_AUTO_TEST_CASE(Async)
{
	std::string msg{"12345"};

	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string first, second;
	first.resize(2 * msg.size());
	second.resize(2 * msg.size());

	auto nfirst = stream.async_read_some(
			asio::buffer(first),
			asio::use_future
			);

	stream.write_some(asio::buffer(msg));

	io.run();

	first.resize(nfirst.get());
	BOOST_TEST(first == msg);

	io.restart();

	auto nsecond = stream.async_read_some(
			asio::buffer(second),
			asio::use_future
			);

	stream.write_some(asio::buffer(msg));

	io.run();

	second.resize(nsecond.get());
	BOOST_TEST(second == msg);
}

BOOST_AUTO_TEST_SUITE_END() // SequentialWritesResultInSequentialReads

BOOST_AUTO_TEST_CASE(BufSizeZeroCompletesImmediately) 
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string empty;
	auto n = stream.async_read_some(asio::buffer(empty), asio::use_future);

	io.run();

	BOOST_TEST(n.get() == 0);
}

BOOST_AUTO_TEST_SUITE(ReadBufferSmallerThanQueuedOp)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	
	std::string msg{"ab"};
	stream.write_some(asio::buffer(msg));

	char a = 0, b = 0;

	auto nfirst =
		stream.async_read_some(asio::buffer(&a, sizeof(char)), asio::use_future);

	io.run();

	BOOST_TEST(nfirst.get() == sizeof(char));
	BOOST_TEST(a == 'a');

	io.restart();

	auto nsecond =
		stream.async_read_some(asio::buffer(&b, sizeof(char)), asio::use_future);

	io.run();

	BOOST_TEST(nsecond.get() == sizeof(char));
	BOOST_TEST(b == 'b');
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	
	std::string msg{"ab"};

	char a = 0, b = 0;

	auto nfirst =
		stream.async_read_some(asio::buffer(&a, sizeof(char)), asio::use_future);

	stream.write_some(asio::buffer(msg));

	io.run();

	BOOST_TEST(nfirst.get() == sizeof(char));
	BOOST_TEST(a == 'a');

	io.restart();

	auto nsecond =
		stream.async_read_some(asio::buffer(&b, sizeof(char)), asio::use_future);

	io.run();

	BOOST_TEST(nsecond.get() == sizeof(char));
	BOOST_TEST(b == 'b');
}

BOOST_AUTO_TEST_SUITE_END() // ReadBufferSmallerThanQueuedOp

BOOST_AUTO_TEST_SUITE(ErrorInStream)

BOOST_AUTO_TEST_SUITE(EmptyStream)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	stream.error(asio::error::eof);

	std::string nonempty{"not empty"};

	boost::system::error_code the_err;
	std::size_t nread = 1;

	stream.async_read_some(asio::buffer(nonempty), [&]
		(const boost::system::error_code &ec, std::size_t n)
		{
			the_err = ec;
			nread = n;
		});

	io.run();

	BOOST_TEST(the_err == asio::error::eof);
	BOOST_TEST(nread == 0);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string nonempty{"not empty"};

	boost::system::error_code the_err;
	std::size_t nread = 1;

	stream.async_read_some(asio::buffer(nonempty), [&]
		(const boost::system::error_code &ec, std::size_t n)
		{
			the_err = ec;
			nread = n;
		});

	stream.error(asio::error::eof);

	io.run();

	BOOST_TEST(the_err == asio::error::eof);
	BOOST_TEST(nread == 0);
}

BOOST_AUTO_TEST_SUITE_END() // EmptyStream

BOOST_AUTO_TEST_SUITE(NonEmptyStream)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string nonempty{"not empty"};

	stream.write_some(asio::buffer(nonempty));
	stream.error(asio::error::fault);

	boost::system::error_code the_err;
	std::size_t nread = 1;

	auto n = stream.async_read_some(asio::buffer(nonempty), asio::use_future);

	io.run();
	n.get();

	io.restart();

	stream.async_read_some(asio::buffer(nonempty), [&]
		(const boost::system::error_code &ec, std::size_t n)
		{
			the_err = ec;
			nread = n;
		});

	io.run();

	BOOST_TEST(the_err == asio::error::fault);
	BOOST_TEST(nread == 0);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	std::string nonempty{"not empty"};

	boost::system::error_code the_err;
	std::size_t nread = 1;

	auto n = stream.async_read_some(asio::buffer(nonempty), asio::use_future);

	stream.write_some(asio::buffer(nonempty));

	io.run();
	n.get();

	io.restart();

	stream.async_read_some(asio::buffer(nonempty), [&]
		(const boost::system::error_code &ec, std::size_t n)
		{
			the_err = ec;
			nread = n;
		});

	stream.error(asio::error::fault);

	io.run();

	BOOST_TEST(the_err == asio::error::fault);
	BOOST_TEST(nread == 0);
}

BOOST_AUTO_TEST_SUITE_END() // NonEmptyStream

BOOST_AUTO_TEST_SUITE_END() // ErrorInStreamIsRead

BOOST_AUTO_TEST_SUITE_END() // Reading
