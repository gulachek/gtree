#define BOOST_TEST_MODULE Base128Test
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/base128.hpp"
#include <sstream>
#include <vector>
#include <cstdint>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(ReadEmptyIsEof)
{
	std::istringstream ss;

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!!err);
	BOOST_TEST(err.is_eof());
}

BOOST_AUTO_TEST_CASE(ReadSingleByteValue)
{
	std::istringstream ss{"\x0a"};

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 0x0a);
}

BOOST_AUTO_TEST_CASE(ReadMultiByteValue)
{
	std::istringstream ss{"\x81\x02"};

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 257);
}

BOOST_AUTO_TEST_CASE(IncompleteIsError)
{
	std::istringstream ss{"\x81\x82"};

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!!err);
	BOOST_TEST(err.ucode() == gt::read_base128_error::incomplete);
}

BOOST_AUTO_TEST_CASE(BadStreamIsError)
{
	std::istringstream ss{"\x81\x82"};
	ss.setstate(std::istream::failbit);

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!!err);
	BOOST_TEST(err.ucode() == gt::read_base128_error::bad_stream);
}

BOOST_AUTO_TEST_CASE(MaxValIsNotError)
{
	// list out assumption
	BOOST_REQUIRE(sizeof(std::size_t) == 8);

	std::istringstream ss{"\xff\xff\xff\xff\xff\xff\xff\xff\xff\x01"};

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 0xffffffffffffffffLU);
}

BOOST_AUTO_TEST_CASE(OverflowIsError)
{
	// list out assumption
	BOOST_REQUIRE(sizeof(std::size_t) == 8);

	std::istringstream ss{"\xff\xff\xff\xff\xff\xff\xff\xff\xff\x02"};

	std::size_t n;
	auto err = gt::read_base128(ss, &n);

	BOOST_TEST(!!err);
	BOOST_TEST(err.ucode() == gt::read_base128_error::overflow);
}
