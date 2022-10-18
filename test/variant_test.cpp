#define BOOST_TEST_MODULE VariantTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/variant.hpp"

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
	tr.child_count(1);
	std::size_t n = 3;
	cvt(n, &tr.child(0));

	std::variant<std::size_t, std::string> v;

	auto err = cvt(tr, &v);

	BOOST_TEST(!err);

	BOOST_TEST(std::get<std::size_t>(v) == 3);
}

BOOST_AUTO_TEST_CASE(ErrorDecodingAltIndexIsError)
{
	vec index;
	index.resize(sizeof(std::size_t) + 1);
	for (std::size_t i = 0; i <= sizeof(std::size_t); ++i)
		index[i] = 0;

	gt::tree tr{ index };

	tr.child_count(1);
	std::size_t n = 3;
	cvt(n, &tr.child(0));

	std::variant<std::size_t, std::string> v;

	auto err = cvt(tr, &v);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(AltOutOfBoundsIsError)
{
	vec index = { 2 };
	gt::tree tr{ index };

	tr.child_count(1);
	std::size_t n = 3;
	cvt(n, &tr.child(0));

	std::variant<std::size_t, std::string> v;

	auto err = cvt(tr, &v);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(ErrorDecodingAltIsError)
{
	vec index = { 1 };
	gt::tree tr{ index };

	tr.child_count(1);
	std::size_t n = 3;
	cvt(n, &tr.child(0));

	std::variant<std::size_t, saboteur> v;

	auto err = cvt(tr, &v);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(NoChildrenIsError)
{
	vec index = { 1 };
	gt::tree tr{ index };

	std::variant<std::size_t, std::string> v;

	auto err = cvt(tr, &v);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeStrint)
{

	std::variant<std::size_t, std::string> v = "hello";

	gt::tree tr;
	auto err = cvt(v, &tr);

	BOOST_REQUIRE(!err);

	std::size_t index = 0;
	std::string hello;
	cvt(tr, &index);
	cvt(tr.child(0), &hello); 

	BOOST_TEST(index == 1);
	BOOST_TEST(hello == "hello");
}

BOOST_AUTO_TEST_CASE(ErrorEncodingAltIsError)
{
	std::variant<std::size_t, saboteur> v = saboteur{true};
	std::stringstream ss;

	auto err = gt::write(ss, v);

	BOOST_TEST(!!err);
}
