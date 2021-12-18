#define BOOST_TEST_MODULE PairEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/pair.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/tree.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace gt = gulachek::gtree;

using uint = unsigned int;
using tree = gt::mutable_tree;

BOOST_AUTO_TEST_CASE(Encode)
{
	std::pair<uint, uint> pair = {1, 2};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	BOOST_REQUIRE(tr.child_count() == 2);
	uint first = 0, second = 0;
	gt::decode(tr.child(0), first);
	gt::decode(tr.child(1), second);

	BOOST_TEST(first == 1);
	BOOST_TEST(second == 2);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	std::pair<uint, uint> truth = {1, 2};
	tree tr;
	gt::encode(truth, tr);

	std::pair<uint, uint> pair = {0,0};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(pair.second == 2);
}
