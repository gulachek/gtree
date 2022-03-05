#define BOOST_TEST_MODULE TreeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

namespace tt = boost::test_tools;

#include "saboteur.hpp"

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/map.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DefaultTreeIsEmpty)
{
	gt::tree tr;
	BOOST_TEST(tr.empty());
}

BOOST_AUTO_TEST_CASE(OptimizeReadInt)
{
	gt::tree tr;
	int n = 10;

	gt::translate(n, &tr);

	int out = 0;
	auto err = tr.read(&out);

	BOOST_TEST(!err);
	BOOST_TEST(out == 10);
}

BOOST_AUTO_TEST_CASE(OptimizeReadVector)
{
	gt::tree tr;
	std::vector<int> v = { 1, 2, 3 };

	gt::translate(v, &tr);

	std::vector<int> out;
	auto err = tr.read(&out);

	BOOST_TEST(!err);
	BOOST_TEST(out == v, tt::per_element());
}

BOOST_AUTO_TEST_CASE(OptimizeReadMap)
{
	gt::tree tr;
	std::map<std::string, int> m = {
		{"foo", 1},
		{"bar", 2}
	};

	gt::translate(m, &tr);

	std::map<std::string, int> out;
	auto err = tr.read(&out);

	BOOST_TEST(!err);
	BOOST_TEST(out.size() == 2);
	BOOST_TEST(out["foo"] == 1);
	BOOST_TEST(out["bar"] == 2);
}

BOOST_AUTO_TEST_CASE(OptimizeReadErrorIsError)
{
	gt::tree tr;
	std::map<std::string, int> m = {
		{"foo", 1},
		{"bar", 2}
	};

	gt::translate(m, &tr);

	std::map<std::string, saboteur> out;
	auto err = tr.read(&out);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(OptimizeWriteInt)
{
	gt::tree tr;
	int n = 10;

	auto err = tr.write(n);
	BOOST_REQUIRE(!err);

	int out = 0;
	gt::translate(tr, &out);

	BOOST_TEST(out == 10);
}

BOOST_AUTO_TEST_CASE(OptimizeWriteVec)
{
	gt::tree tr;
	std::vector<int> v = { 1, 2, 3 };

	auto err = tr.write(v);
	BOOST_REQUIRE(!err);

	std::vector<int> out;
	gt::translate(tr, &out);

	BOOST_TEST(out == v, tt::per_element());
}

BOOST_AUTO_TEST_CASE(OptimizeWriteMap)
{
	gt::tree tr;
	std::map<std::string, int> m = {
		{"foo", 1},
		{"bar", 2}
	};

	auto err = tr.write(m);
	BOOST_REQUIRE(!err);

	std::map<std::string, int> out;
	gt::translate(tr, &out);

	BOOST_TEST(out.size() == 2);
	BOOST_TEST(out["foo"] == 1);
	BOOST_TEST(out["bar"] == 2);
}

BOOST_AUTO_TEST_CASE(OptimizeWriteErrorIsError)
{
	gt::tree tr;
	std::map<std::string, saboteur> m = {
		{"foo", {false}},
		{"bar", {true}}
	};

	auto err = tr.write(m);

	BOOST_TEST(!!err);
}
