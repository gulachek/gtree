#define BOOST_TEST_MODULE gnatz_readerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gnatz_reader.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <sstream>

namespace gt = gulachek::gtree;

using Data = std::vector<std::uint8_t>;

auto vec_stream(const Data &vec)
{
	std::stringstream _ss;
	_ss.write((char *)vec.data(), vec.size());
	return _ss;
}

BOOST_AUTO_TEST_SUITE(ReadingNums)

std::size_t lengths[] = {1, 2, 3, 4, 5, 6, 7, 8};

BOOST_DATA_TEST_CASE(
		Zero,
		bd::make(lengths),
		length
		)
{
	Data raw(length, 0x80);
	raw.back() = 0;
	auto stream = vec_stream(raw);

	gt::gnatz_reader gnz{stream};
	std::size_t n;
	bool read = gnz.read(&n);

	BOOST_TEST(read);
	BOOST_TEST(n == 0);
}

BOOST_DATA_TEST_CASE(
		One,
		bd::make(lengths),
		length
		)
{
	Data raw(length, 0x80);
	raw.front() = 1;
	if (length > 1) raw.back() = 0;

	auto stream = vec_stream(raw);
	gt::gnatz_reader gnz{stream};

	std::size_t n;
	auto read = gnz.read(&n);

	BOOST_TEST(read);
	BOOST_TEST(n == 1);
}

BOOST_AUTO_TEST_CASE(TwoInARow)
{
	Data raw = {1, 2};
	auto stream = vec_stream(raw);

	gt::gnatz_reader gnz{stream};

	std::size_t first, second;
	BOOST_TEST(gnz.read(&first));
	BOOST_TEST(gnz.read(&second));
	BOOST_TEST(first == 1);
	BOOST_TEST(second == 2);
}

BOOST_AUTO_TEST_CASE(TwoFiftySix)
{
	Data raw = {0x80, 0x02};
	auto stream = vec_stream(raw);

	gt::gnatz_reader gnz{stream};

	std::size_t n;
	BOOST_TEST(gnz.read(&n));
	BOOST_TEST(n == 256);
}

BOOST_AUTO_TEST_CASE(FifteenBitsHigh)
{
	Data raw = {0xff, 0xff, 0x01};
	auto stream = vec_stream(raw);

	gt::gnatz_reader gnz{stream};

	std::size_t n;
	BOOST_TEST(gnz.read(&n));
	BOOST_TEST(n == 0x7fff);
}

BOOST_AUTO_TEST_SUITE_END() // ReadingNums

BOOST_AUTO_TEST_SUITE(ProtocolIssues)

BOOST_AUTO_TEST_CASE(IncompleteStreamThrows)
{
	Data raw = {0x80};
	auto stream = vec_stream(raw);

	gt::gnatz_reader gnz{stream};

	std::size_t n;
	BOOST_CHECK_THROW(gnz.read(&n), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EndOfLifetime)

BOOST_AUTO_TEST_CASE(ReadingClosedStreamIsEOF)
{
	std::stringstream stream;

	gt::gnatz_reader gnz{stream};

	std::size_t n;
	BOOST_TEST(!gnz.read(&n));
}

BOOST_AUTO_TEST_CASE(CompleteThenClosedIsNumThenEOF)
{
	Data buf = {32};
	auto stream = vec_stream(buf);

	gt::gnatz_reader gnz{stream};

	std::size_t n;
	BOOST_TEST(gnz.read(&n));
	BOOST_TEST(!gnz.read(&n));
}

BOOST_AUTO_TEST_SUITE_END()
