#define BOOST_TEST_MODULE ListTest
#include <boost/test/unit_test.hpp>

#include "saboteur.hpp"

#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/encoding/list.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<int>;
using lst = std::list<int>;

BOOST_AUTO_TEST_CASE(DecodeNonempty)
{
	vec v = { 1, 2, 3 };
	lst l;

	auto err = gt::translate(v, &l);

	BOOST_TEST(!err);
	lst expect = { 1, 2, 3 };
	BOOST_TEST(l == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	vec v;
	lst l = { 1, 2, 3 };

	auto err = gt::translate(v, &l);

	BOOST_TEST(!err);
	BOOST_TEST(l.empty());
}

BOOST_AUTO_TEST_CASE(EncodeNonempty)
{
	lst l = { 1, 2, 3 };
	vec v;

	auto err = gt::translate(l, &v);

	BOOST_TEST(!err);
	vec expect = { 1, 2, 3 };
	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
	lst l;
	vec v = { 1, 2, 3 };

	auto err = gt::translate(l, &v);

	BOOST_TEST(!err);
	BOOST_TEST(v.empty());
}
