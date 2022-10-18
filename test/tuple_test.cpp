#define BOOST_TEST_MODULE TupleTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree.hpp"
#include "gulachek/gtree/encoding/tuple.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using gulachek::error;
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

// Classes
class strint
{
	std::string str_;
	std::size_t int_;

	public:
		strint() = default;

		std::string s() const { return str_; }
		std::string s(std::string s) { return str_ = std::move(s); }

		std::size_t n() const { return int_; }
		std::size_t n(std::size_t n) { return int_ = n; }

		GTREE_TUPLE_MEMBERS(
				&strint::str_,
				&strint::int_
				);
};

BOOST_AUTO_TEST_CASE(EncodeStrintClass)
{
	strint x;
	x.s("hello");
	x.n(3);

	std::tuple<std::string, std::size_t> t;
	auto err = gt::translate(x, &t);

	BOOST_REQUIRE(!err);

	BOOST_TEST(std::get<0>(t) == "hello");
	BOOST_TEST(std::get<1>(t) == 3);
}

BOOST_AUTO_TEST_CASE(DecodeStrintClass)
{
	std::tuple<std::string, std::size_t> t{"hello", 3};
	strint x;

	auto err = gt::translate(t, &x);

	BOOST_REQUIRE(!err);

	BOOST_TEST(x.s() == "hello");
	BOOST_TEST(x.n() == 3);
}

struct my_pair
{
	std::string s;
	std::size_t n;

	GTREE_DECLARE_MEMBER_FNS
};

GTREE_DEFINE_TUPLE_MEMBER_FNS(my_pair, s, n)

BOOST_AUTO_TEST_CASE(HelperFunDecode)
{
	std::tuple<std::string, std::size_t> tup{"hello", 3};
	my_pair p;

	auto err = gt::translate(tup, &p);

	BOOST_REQUIRE(!err);

	BOOST_TEST(p.s == "hello");
	BOOST_TEST(p.n == 3);
}

BOOST_AUTO_TEST_CASE(HelperFunEncode)
{
	my_pair p{"hello", 3};
	std::tuple<std::string, std::size_t> tup;

	auto err = gt::translate(p, &tup);

	BOOST_REQUIRE(!err);

	BOOST_TEST(std::get<0>(tup) == "hello");
	BOOST_TEST(std::get<1>(tup) == 3);
}
