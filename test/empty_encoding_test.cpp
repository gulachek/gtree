#define BOOST_TEST_MODULE EmptyEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/empty.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace gt = gulachek::gtree;

class empty {};

template <>
struct gt::empty_encoding<empty> { using type = empty; };

BOOST_AUTO_TEST_CASE(NoValue)
{
	typename gt::empty_encoding<empty>::type e;
	BOOST_TEST(!gt::uses_value<empty>::value);
}

BOOST_AUTO_TEST_CASE(NoChildren)
{
	BOOST_TEST(!gt::uses_children<empty>::value);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr;

	empty mt;
	gt::decode(tr, mt);

	// as long as this compiles/runs, we're good
}

BOOST_AUTO_TEST_CASE(Encode)
{
	empty mt;

	gt::mutable_tree tr{ {10} };
	gt::encode(mt, tr);

	BOOST_TEST(gt::is_empty(tr));
}
