#define BOOST_TEST_MODULE FlatTreeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/encoding/map.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/flat_tree.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

using namespace std::string_literals;

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

auto make_vec(const gt::flat_tree &tr)
{
	auto val = tr.buf();
	return vec{val.begin(), val.end()};
}

/*
BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	std::size_t n = 0;
	gt::flat_tree tr;

	auto err = cvt(n, &tr);

	BOOST_TEST(!err);
	auto v = make_vec(tr);
	vec expect = { 0, 0 };

	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeEmptyTwice)
{
	std::size_t n = 0;
	gt::flat_tree tr;

	cvt(n, &tr);
	cvt(n, &tr);

	auto v = make_vec(tr);
	vec expect = { 0, 0 };

	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeHello)
{
	std::string s = "hello";
	gt::flat_tree tr;

	auto err = cvt(s, &tr);

	BOOST_TEST(!err);
	auto v = make_vec(tr);
	vec expect = { 5, 'h', 'e', 'l', 'l', 'o', 0 };

	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeVector)
{
	vec vals = { 1, 2, 3 };
	gt::flat_tree tr;

	auto err = cvt(vals, &tr);

	BOOST_TEST(!err);
	auto v = make_vec(tr);
	vec expect = { 0, 3, 1, 1, 0, 1, 2, 0, 1, 3, 0 };

	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
	gt::flat_tree tr;
	std::ostringstream ss;

	auto err = gt::write(ss, tr);

	BOOST_TEST(!err);

	auto s = ss.str();
	BOOST_TEST(s == "\0\0"s);
}

BOOST_AUTO_TEST_CASE(EncodeHello)
{
	std::string hello = "hello";
	gt::flat_tree tr;

	BOOST_TEST(!cvt(hello, &tr));

	std::string out;
	BOOST_TEST(!cvt(tr, &out));

	BOOST_TEST(out == "hello");
}

BOOST_AUTO_TEST_CASE(EncodeVector)
{
	vec vals = { 1, 2, 3 };
	gt::flat_tree tr;

	BOOST_TEST(!cvt(vals, &tr));

	vec out;
	BOOST_TEST(!cvt(tr, &out));

	BOOST_TEST(out == vals, tt::per_element());
}

BOOST_AUTO_TEST_CASE(CanCopyFlatTree)
{
	vec vals = { 1, 2, 3 };
	gt::flat_tree tr;

	tr.write(vals);

	gt::flat_tree cpy{tr};

	vec out;
	tr.read(&out);

	BOOST_TEST(out == vals, tt::per_element());
}
*/

BOOST_AUTO_TEST_CASE(CanMoveConstruct)
{
	BOOST_TEST(std::is_move_constructible_v<gt::flat_tree>);
}
