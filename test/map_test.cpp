#define BOOST_TEST_MODULE MapTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/encoding/map.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/pair.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using gulachek::error;
namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeStrint)
{
	std::vector<std::pair<std::string, unsigned int>> v = {
		{"foo", 1},
		{"bar", 2}
	};

	std::map<std::string, unsigned int> m;

	auto err = cvt(v, &m);

	BOOST_TEST(!err);

	BOOST_TEST(m.size() == 2);
	BOOST_TEST(m["foo"] == 1);
	BOOST_TEST(m["bar"] == 2);
}

BOOST_AUTO_TEST_CASE(DecodeDupKeysIsError)
{
	std::vector<std::pair<std::string, unsigned int>> v = {
		{"foo", 1},
		{"foo", 1}
	};

	std::map<std::string, unsigned int> m;

	auto err = cvt(v, &m);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInElemIsError)
{
	std::vector<std::pair<std::string, std::uint8_t>> v = {
		{"success", 0},
		{"fail", 1}
	};

	std::map<std::string, saboteur> m;

	auto err = cvt(v, &m);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeStrint)
{
	std::map<std::string, unsigned int> m = {
		{"foo", 1},
		{"bar", 2}
	};

	std::vector<std::pair<std::string, unsigned int>> v;

	auto err = cvt(m, &v);

	BOOST_TEST(!err);

	BOOST_TEST(v.size() == 2);
	BOOST_TEST(v[0].first == "bar");
	BOOST_TEST(v[0].second == 2);
	BOOST_TEST(v[1].first == "foo");
	BOOST_TEST(v[1].second == 1);
}

BOOST_AUTO_TEST_CASE(EncodeErrorInElemIsError)
{
	std::map<std::string, saboteur> m = {
		{"foo", {}},
		{"bar", {true}}
	};

	std::vector<std::pair<std::string, unsigned int>> v;

	auto err = cvt(m, &v);

	BOOST_TEST(!!err);
}
