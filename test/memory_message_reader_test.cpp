#define BOOST_TEST_MODULE memory_message_readerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/asio/memory_message_reader.hpp"
#include <type_traits>
#include <tuple>
#include <utility>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(MoveConstructible)
{
	BOOST_TEST(std::is_move_constructible<gta::memory_message_reader<int>>::value);
}

typedef std::tuple<std::string, int, bool> message_ts;

BOOST_AUTO_TEST_CASE_TEMPLATE(SpecifiedAsTemplate, Message, message_ts)
{
	bool same_message = std::is_same<
				Message,
				typename gta::memory_message_reader<Message>::message_type
			>::value;

	BOOST_TEST(same_message);
}

BOOST_AUTO_TEST_CASE(ExecutorTypeDefaultsToAsioExecutor)
{
	bool same_executor = std::is_same<
		asio::executor,
		typename gta::memory_message_reader<int>::executor_type
			>::value;

	BOOST_TEST(same_executor);
}

BOOST_AUTO_TEST_SUITE_END() // TypeTraits

BOOST_AUTO_TEST_SUITE(GetExecutor)

BOOST_AUTO_TEST_CASE(ConstructWithIoContextHasSameGetExecutor)
{
	asio::io_context io;
	gta::memory_message_reader<int> reader{io};

	BOOST_CHECK(io.get_executor() == reader.get_executor());
}

BOOST_AUTO_TEST_SUITE_END() // GetExecutor

BOOST_AUTO_TEST_SUITE(ReadMessage)

BOOST_AUTO_TEST_SUITE(SingleMessage)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_message_reader<int> reader{io};

	reader.write(3);

	auto result = reader.async_read(asio::use_future);

	io.run();

	BOOST_TEST(result.get() == 3);
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_message_reader<std::string> reader{io};

	auto result = reader.async_read(asio::use_future);

	reader.write("hello");

	io.run();

	BOOST_TEST(result.get() == "hello");
}

BOOST_AUTO_TEST_SUITE_END() // SingleMessage

BOOST_AUTO_TEST_SUITE(MultipleMessages)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_message_reader<bool> reader{io};

	reader.write(true);
	reader.write(false);

	auto first = reader.async_read(asio::use_future);
	auto second = reader.async_read(asio::use_future);

	io.run();

	BOOST_TEST(first.get());
	BOOST_TEST(!second.get());
}

BOOST_AUTO_TEST_CASE(Async)
{
	struct Msg { int x; };

	asio::io_context io;
	gta::memory_message_reader<Msg> reader{io};

	auto first = reader.async_read(asio::use_future);
	auto second = reader.async_read(asio::use_future);

	reader.write({3});
	reader.write({24});

	io.run();

	BOOST_TEST(first.get().x == 3);
	BOOST_TEST(second.get().x == 24);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END() // ReadMessage

BOOST_AUTO_TEST_SUITE(Errors)

BOOST_AUTO_TEST_SUITE(NoMessagesInQueue)

BOOST_AUTO_TEST_CASE(Sync)
{
	asio::io_context io;
	gta::memory_message_reader<std::string> reader{io};

	reader.error(asio::error::eof);

	boost::system::error_code ec;
	std::string msg = "not empty";

	reader.async_read([&](const boost::system::error_code &e, std::string &&s)
	{
		ec = e;
		msg = std::move(s);
	});

	io.run();

	BOOST_TEST(ec == asio::error::eof);
	BOOST_TEST(msg.empty()); // default constructed
}

BOOST_AUTO_TEST_CASE(Async)
{
	asio::io_context io;
	gta::memory_message_reader<std::string> reader{io};

	boost::system::error_code ec;
	std::string msg = "not empty";

	reader.async_read([&](const boost::system::error_code &e, std::string &&s)
	{
		ec = e;
		msg = std::move(s);
	});

	reader.error(asio::error::fault);


	io.run();

	BOOST_TEST(ec == asio::error::fault);
	BOOST_TEST(msg.empty()); // default constructed
}

BOOST_AUTO_TEST_SUITE_END() // NoMessagesInQueue

BOOST_AUTO_TEST_SUITE_END() // Errors
