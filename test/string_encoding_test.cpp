#define BOOST_TEST_MODULE StringEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeBinaryString)
{
	gt::mutable_tree tr{ { 'a', 'b', 'c', 255 } };

	std::string str;
	BOOST_TEST(!gt::decode(tr, str));

	BOOST_TEST(str == "abc\xff");
}

BOOST_AUTO_TEST_CASE(EncodeBinaryStrings)
{
	std::string truth = "\xab\xff\x29\xf3hello world";

	gt::mutable_tree tr;
	BOOST_TEST(!gt::encode(truth, tr));

	std::string str;
	gt::decode(tr, str);

	BOOST_TEST(str == truth);
}
