#define BOOST_TEST_MODULE TupleTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/tuple.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeEmptyTup)
{
	gt::tree tr;
	std::tuple<> tup;

	auto err = cvt(tr, &tup);

	BOOST_TEST(!err);
}

BOOST_AUTO_TEST_CASE(DecodeStrint)
{
	gt::tree tr;
	std::tuple<std::string, unsigned int> tup;

	std::string hello = "hello";
	unsigned int n = 3;

	tr.child_count(2);
	cvt(hello, &tr.child(0));
	cvt(n, &tr.child(1));

	auto err = cvt(tr, &tup);

	BOOST_TEST(!err);

	BOOST_TEST(std::get<0>(tup) == "hello");
	BOOST_TEST(std::get<1>(tup) == 3);
}

BOOST_AUTO_TEST_CASE(NotEnoughChildrenIsError)
{
	gt::tree tr;
	std::tuple<std::string, unsigned int> tup;

	auto err = cvt(tr, &tup);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInElemIsError)
{
	gt::tree tr;
	std::tuple<std::string, saboteur> tup;

	tr.child_count(2);
	std::uint8_t n = 1;
	cvt(n, &tr.child(1));

	auto err = cvt(tr, &tup);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeEmptyTuple)
{
	std::tuple<> tup;

	vec val = { 1 };
	gt::tree tr{ val };

	auto err = cvt(tup, &tr);

	BOOST_REQUIRE(!err);

	BOOST_TEST(tr.empty());
}

BOOST_AUTO_TEST_CASE(EncodeStrint)
{
	std::tuple<std::string, unsigned int> tup{"hello", 3};

	gt::tree tr;

	auto err = cvt(tup, &tr);

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

BOOST_AUTO_TEST_CASE(EncodeErrorInElemIsError)
{
	std::tuple<std::string, saboteur> tup{"hello", {true}};

	gt::tree tr;
	auto err = cvt(tup, &tr);

	BOOST_TEST(!!err);
}
