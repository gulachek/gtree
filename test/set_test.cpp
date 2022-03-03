#define BOOST_TEST_MODULE SetTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/encoding/set.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/vector.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

struct saboteur_compare
{
	bool operator() (const saboteur &l, const saboteur &r) const
	{
		return ((int)l.error) < ((int)r.error);
	}
};

BOOST_AUTO_TEST_CASE(DecodeInts)
{
	std::vector<int> v = { 1, 2, 3, 3 };
	std::set<int> s;

	auto err = cvt(v, &s);

	BOOST_TEST(!err);

	BOOST_TEST(s.size() == 3);
	BOOST_TEST(s.count(1));
	BOOST_TEST(s.count(2));
	BOOST_TEST(s.count(3));
}

BOOST_AUTO_TEST_CASE(DecodeErrorInElemIsErr)
{
	std::vector<int> v = { 1 };
	std::set<saboteur, saboteur_compare> s;

	auto err = cvt(v, &s);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeInts)
{
	std::set<int> s = { 1, 2, 3 };
	std::vector<int> v;
	std::vector<int> expect = { 1, 2, 3 };

	auto err = cvt(s, &v);

	BOOST_TEST(!err);
	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(EncodeErrorInElemIsErr)
{
	std::set<saboteur, saboteur_compare> s = { {true} };
	std::stringstream ss;

	auto err = gt::write(ss, s);

	BOOST_TEST(!!err);
}
