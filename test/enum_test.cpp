#define BOOST_TEST_MODULE EnumTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/enum.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

enum class unsign : std::uint8_t
{
	two_fifty_five = 255,
};

enum class sign : std::int8_t
{
	negative_one = -1
};

BOOST_AUTO_TEST_CASE(DecodeUnsignedValue)
{
	unsign u = (unsign)0;
	std::uint8_t n = 255;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &u);

	BOOST_TEST(!err);
	BOOST_CHECK(u == unsign::two_fifty_five);
}

BOOST_AUTO_TEST_CASE(DecodeSignedValue)
{
	std::int8_t n = -1;
	sign s = (sign)0;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &s);

	BOOST_TEST(!err);
	BOOST_CHECK(s == sign::negative_one);
}

BOOST_AUTO_TEST_CASE(EncodeUnsignedValue)
{
	unsign u = unsign::two_fifty_five;
	std::uint8_t n = 0;

	std::stringstream ss;
	auto err = gt::write(ss, u);
	BOOST_REQUIRE(!err);

	gt::read(ss, &n);

	BOOST_TEST(n == 255);
}

BOOST_AUTO_TEST_CASE(EncodeSignedValue)
{
	sign s = sign::negative_one;
	std::int8_t n = 0;

	std::stringstream ss;
	auto err = gt::write(ss, s);
	BOOST_REQUIRE(!err);

	gt::read(ss, &n);

	BOOST_TEST(n == -1);
}
