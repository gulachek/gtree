#define BOOST_TEST_MODULE EmptyEncodingTest
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

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<gt::empty>::value);
}

BOOST_AUTO_TEST_CASE(NoChildren)
{
	BOOST_TEST(!gt::uses_children<gt::empty>::value);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr;

	gt::empty empty;
	gt::decode(tr, empty);

	// as long as this compiles/runs, we're good
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::empty empty;

	gt::mutable_tree tr{ {10} };
	gt::encode(empty, tr);

	BOOST_TEST(gt::is_empty(tr));
}
