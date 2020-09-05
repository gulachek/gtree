#define BOOST_TEST_MODULE Asyncgnatz_readerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/asio/gnatz_reader.hpp"
#include "gulachek/gtree/asio/memory_read_stream.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

using Data = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_SUITE(ReadingNumbers)

std::size_t lengths[] = {1, 2, 3, 4, 5, 6, 7, 8};

BOOST_AUTO_TEST_SUITE(Construction)

BOOST_AUTO_TEST_CASE(MoveConstructible)
{
	BOOST_TEST(std::is_move_constructible<
			gta::gnatz_reader<gta::memory_read_stream<asio::executor>>
			>::value);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Zeros)

BOOST_DATA_TEST_CASE(
		Sync,
		bd::make(lengths),
		length
		)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw(length, 0x80);
	raw.back() = 0;

	asio::write(stream, asio::buffer(raw));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);

	io.run();

	BOOST_TEST(n.get() == 0);
}

BOOST_DATA_TEST_CASE(
		Async,
		bd::make(lengths),
		length
		)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw(length, 0x80);
	raw.back() = 0;

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);

	asio::write(stream, asio::buffer(raw));

	io.run();

	BOOST_TEST(n.get() == 0);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(One)

BOOST_DATA_TEST_CASE(
		Sync,
		bd::make(lengths),
		length
		)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw(length, 0x80);
	raw.front() = 1;
	if (length > 1) raw.back() = 0;

	asio::write(stream, asio::buffer(raw));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);

	io.run();

	BOOST_TEST(n.get() == 1);
}

BOOST_DATA_TEST_CASE(
		Async,
		bd::make(lengths),
		length
		)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw(length, 0x80);
	raw.front() = 1;
	if (length > 1) raw.back() = 0;

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);

	asio::write(stream, asio::buffer(raw));

	io.run();

	BOOST_TEST(n.get() == 1);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(TwoInARow)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw = {1, 2};

	asio::write(stream, asio::buffer(raw));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto first = gnz.async_read(asio::use_future);
	io.run();
	BOOST_TEST(first.get() == 1);

	io.restart();

	auto second = gnz.async_read(asio::use_future);
	io.run();
	BOOST_TEST(second.get() == 2);
}

BOOST_AUTO_TEST_SUITE(TwoFiftySix)

BOOST_AUTO_TEST_SUITE(OneShot)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw = {0x80, 0x02};

	asio::write(stream, asio::buffer(raw));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);
	io.run();
	BOOST_TEST(n.get() == 256);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw = {0x80, 0x02};

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);

	asio::write(stream, asio::buffer(raw));

	io.run();
	BOOST_TEST(n.get() == 256);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(Incremental)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::uint8_t first = 0x80, second = 0x02;

	asio::write(stream, asio::buffer(&first, 1));
	asio::write(stream, asio::buffer(&second, 1));
	auto n = gnz.async_read(asio::use_future);

	io.run();

	BOOST_TEST(n.get() == 256);
}

BOOST_AUTO_TEST_CASE(Mixed)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::uint8_t first = 0x80, second = 0x02;

	asio::write(stream, asio::buffer(&first, 1));
	auto n = gnz.async_read(asio::use_future);
	asio::write(stream, asio::buffer(&second, 1));

	io.run();

	BOOST_TEST(n.get() == 256);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::uint8_t first = 0x80, second = 0x02;

	auto n = gnz.async_read(asio::use_future);
	asio::write(stream, asio::buffer(&first, 1));
	asio::write(stream, asio::buffer(&second, 1));

	io.run();

	BOOST_TEST(n.get() == 256);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(FifteenBitsHigh)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw = {0xff, 0xff, 0x01};

	asio::write(stream, asio::buffer(raw));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto n = gnz.async_read(asio::use_future);
	io.run();
	BOOST_TEST(n.get() == 0x7fff);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ProtocolIssues)

BOOST_AUTO_TEST_CASE(IncompleteStreamIsBadMessage)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	Data raw = {0x80};

	asio::write(stream, asio::buffer(raw));
	stream.error(asio::error::eof);

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::size_t the_num = 1;
	boost::system::error_code the_ec;

	gnz.async_read([&]
			(const boost::system::error_code &ec, std::size_t n)
			{
				the_num = n;
				the_ec = ec;
			});

	io.run();

	BOOST_TEST(the_num == 0);
	BOOST_TEST(the_ec == gta::error::bad_message);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(EndOfLifetime)

BOOST_AUTO_TEST_SUITE(ReadingClosedStreamIsEOF)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	stream.error(asio::error::eof);

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::size_t the_num = 1;
	boost::system::error_code the_ec;

	gnz.async_read([&]
			(const boost::system::error_code &ec, std::size_t n)
			{
				the_num = n;
				the_ec = ec;
			});

	io.run();

	BOOST_TEST(the_num == 0);
	BOOST_TEST(the_ec == boost::asio::error::eof);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	std::size_t the_num = 1;
	boost::system::error_code the_ec;

	gnz.async_read([&]
			(const boost::system::error_code &ec, std::size_t n)
			{
				the_num = n;
				the_ec = ec;
			});

	stream.error(asio::error::eof);

	io.run();

	BOOST_TEST(the_num == 0);
	BOOST_TEST(the_ec == boost::asio::error::eof);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(CompleteThenClosedIsNumThenEOF)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	Data buf = {32};
	asio::write(stream, asio::buffer(buf));
	stream.error(asio::error::eof);

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto first = gnz.async_read(asio::use_future);
	io.run();
	BOOST_TEST(first.get() == 32);

	io.restart();

	std::size_t second = 1;
	boost::system::error_code the_ec;

	gnz.async_read([&]
			(const boost::system::error_code &ec, std::size_t n)
			{
				second = n;
				the_ec = ec;
			});

	io.run();

	BOOST_TEST(second == 0);
	BOOST_TEST(the_ec == boost::asio::error::eof);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(UsingExternalStreamBuf)

BOOST_AUTO_TEST_CASE(ExtraBytesStayInBuffer)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};

	Data buf = {32, 64};
	asio::write(stream, asio::buffer(buf));

	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	auto first = gnz.async_read(asio::use_future);
	io.run();
	first.get();

	BOOST_TEST(sb.size() == 1);

	std::uint8_t n = 0;
	asio::buffer_copy(asio::buffer(&n, 1), sb.data());

	BOOST_TEST(n == 64);
}

BOOST_AUTO_TEST_CASE(BufferAlreadyHasBytesThenUsesThemFirst)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	asio::streambuf sb;
	gta::gnatz_reader gnz{stream, &sb};

	Data pre_existing = {0x80, 0x80};
	auto n = asio::buffer_copy(sb.prepare(2), asio::buffer(pre_existing));
	sb.commit(n);

	Data buf = {32, 64};
	asio::write(stream, asio::buffer(buf));

	auto num = gnz.async_read(asio::use_future);
	io.run();

	BOOST_TEST(num.get() == (32 << 14));
	BOOST_TEST(sb.size() == 1); // left over
}

BOOST_AUTO_TEST_SUITE_END() // UsingExternalBuffer
