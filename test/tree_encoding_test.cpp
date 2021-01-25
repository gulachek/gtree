#define BOOST_TEST_MODULE TreeEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <variant>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<gt::tree>::value);
	BOOST_TEST(gt::uses_value<gt::mutable_tree>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<gt::tree>::value);
	BOOST_TEST(gt::uses_children<gt::mutable_tree>::value);
}

BOOST_AUTO_TEST_CASE(CopyOnDecode)
{
	gt::mutable_tree src{{1, 2, 3}}, dest;

	gt::decode(src, dest);

	BOOST_CHECK(src == dest);
}

BOOST_AUTO_TEST_CASE(CopyOnEncode)
{
	gt::mutable_tree src{{}, {{{1, 2, 3}}}}, dest;

	gt::encode(src, dest);

	BOOST_CHECK(src == dest);
}
