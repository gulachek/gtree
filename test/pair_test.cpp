#define BOOST_TEST_MODULE PairTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/pair.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using gulachek::error;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeStrint)
{
	gt::tree tr;
	std::pair<std::string, unsigned int> pair;

	std::string hello = "hello";
	unsigned int n = 3;

	tr.child_count(2);
	cvt(hello, &tr.child(0));
	cvt(n, &tr.child(1));

	auto err = cvt(tr, &pair);

	BOOST_TEST(!err);

	BOOST_TEST(pair.first == "hello");
	BOOST_TEST(pair.second == 3);
}

BOOST_AUTO_TEST_CASE(NotEnoughChildrenIsError)
{
	gt::tree tr;
	std::pair<std::string, unsigned int> pair;

	auto err = cvt(tr, &pair);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInFirstElemIsError)
{
	gt::tree tr;
	std::pair<saboteur, std::string> pair;

	tr.child_count(2);
	std::uint8_t n = 1;
	cvt(n, &tr.child(0));

	auto err = cvt(tr, &pair);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInSecondElemIsError)
{
	gt::tree tr;
	std::pair<std::string, saboteur> pair;

	tr.child_count(2);
	std::uint8_t n = 1;
	cvt(n, &tr.child(1));

	auto err = cvt(tr, &pair);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeStrint)
{
	std::pair<std::string, unsigned int> pair{"hello", 3};

	gt::tree tr;

	auto err = cvt(pair, &tr);

	BOOST_REQUIRE(!err);

	std::string s;
	unsigned int n = 0;

	BOOST_TEST(tr.value().empty());
	BOOST_TEST(tr.child_count() == 2);
	
	cvt(tr.child(0), &s);
	cvt(tr.child(1), &n);

	BOOST_TEST(s == "hello");
	BOOST_TEST(n == 3);
}

BOOST_AUTO_TEST_CASE(EncodeErrorInFirstElemIsError)
{
	std::pair<saboteur, std::string> pair{{true}, "hello"};

	gt::tree tr;
	auto err = cvt(pair, &tr);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeErrorInSecondElemIsError)
{
	std::pair<std::string, saboteur> pair{"hello", {true}};

	gt::tree tr;
	auto err = cvt(pair, &tr);

	BOOST_TEST(!!err);
}
