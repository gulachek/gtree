#define BOOST_TEST_MODULE OptionalTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/optional.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeEmptyIsNullopt)
{
	std::vector<int> v;
	std::optional<int> o = 3;

	auto err = cvt(v, &o);

	BOOST_TEST(!err);
	BOOST_TEST(!o);
}

BOOST_AUTO_TEST_CASE(DecodeNonNull)
{
	std::vector<int> v = {3};
	std::optional<int> o;

	auto err = cvt(v, &o);

	BOOST_TEST(!err);
	BOOST_TEST(o.has_value());
	BOOST_TEST(*o == 3);
}

BOOST_AUTO_TEST_CASE(DecodeErrInValueIsErr)
{
	std::vector<int> v = {3};
	std::optional<saboteur> o;

	auto err = cvt(v, &o);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeNulloptIsEmpty)
{
	std::optional<int> o;
	std::vector<int> v = {3};

	auto err = cvt(o, &v);

	BOOST_TEST(!err);
	BOOST_TEST(v.empty());
}

BOOST_AUTO_TEST_CASE(EncodeValueIsNonempty)
{
	std::optional<int> o = 3;
	std::vector<int> v;

	auto err = cvt(o, &v);

	BOOST_TEST(!err);
	BOOST_TEST(v.size() == 1);
	BOOST_TEST(v[0] == 3);
}

BOOST_AUTO_TEST_CASE(EncodeErrorInValIsError)
{
	std::optional<saboteur> o = saboteur{true};
	std::stringstream ss;

	auto err = gt::write(ss, o);

	BOOST_TEST(!!err);
}
