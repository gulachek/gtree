#define BOOST_TEST_MODULE EnumEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/optimization_type.hpp"

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(EncodeDecodeCompiles)
{
	gt::optimization_type ot;
	gt::mutable_tree tr;
	gt::encode(ot, tr);
	gt::decode(tr, ot);
}
