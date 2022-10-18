#define BOOST_TEST_MODULE SharedPtrTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/shared_ptr.hpp"

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using gulachek::error;
namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeValueAsPointer)
{
	int n = 3;
	std::shared_ptr<int> p;

	auto err = cvt(n, &p);

	BOOST_TEST(!err);
	BOOST_TEST(*p == 3);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInValueIsError)
{
	int n = 3;
	std::shared_ptr<saboteur> p;

	auto err = cvt(n, &p);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodePointerAsValue)
{
	std::shared_ptr<int> p = std::make_shared<int>(3);
	int n = 0;

	auto err = cvt(p, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 3);
}

BOOST_AUTO_TEST_CASE(EncodeNullIsError)
{
	std::shared_ptr<int> p;
	std::stringstream ss;

	auto err = gt::write(ss, p);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeErrorInValIsError)
{
	auto p = std::make_shared<saboteur>(saboteur{true});
	std::stringstream ss;

	auto err = gt::write(ss, p);

	BOOST_TEST(!!err);
}
