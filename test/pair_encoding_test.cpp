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
using vec = std::vector<uint>;
using tree = gt::mutable_tree;

using pval_pval = std::pair<uint, uint>;
using pval_pcon = std::pair<uint, vec>;
using pval_gval = std::pair<uint, pval_pval>;
using pcon_pval = std::pair<vec, uint>;
using gval_pval = std::pair<pval_pval, uint>;
using pcon_pcon = std::pair<vec, vec>;

vec v123 = {1, 2, 3};
vec v456 = {4, 5, 6};

BOOST_AUTO_TEST_SUITE(PValPVal)

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<pval_pval>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<pval_pval>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	pval_pval pair = {1, 2};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	BOOST_REQUIRE(tr.child_count() == 1);
	uint first = 0, second = 0;
	gt::decode(tr, first);
	gt::decode(tr.child(0), second);

	BOOST_TEST(first == 1);
	BOOST_TEST(second == 2);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	pval_pval truth = {1, 2};
	tree tr;
	gt::encode(truth, tr);

	pval_pval pair = {0,0};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(pair.second == 2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PValPCon)

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<pval_pcon>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<pval_pcon>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	pval_pcon pair = {1, v123};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	uint n = 0;
	vec v;
	gt::decode(tr, n);
	gt::decode(tr, v);

	BOOST_TEST(n == 1);
	BOOST_TEST(v == v123, tt::per_element());
}

BOOST_AUTO_TEST_CASE(Decode)
{
	pval_pcon truth = {1, v123};
	tree tr;
	gt::encode(truth, tr);

	pval_pcon pair = {0,{}};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(pair.second == v123, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PValGVal)

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<pval_gval>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<pval_gval>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	pval_gval pair = {1, {2, 3}};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	BOOST_REQUIRE(tr.child_count() == 1);

	uint a = 0;
	pval_pval bc = {0,0};

	gt::decode(tr, a);
	gt::decode(tr.child(0), bc);

	BOOST_TEST(a == 1);
	BOOST_TEST(bc.first == 2);
	BOOST_TEST(bc.second == 3);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	pval_gval truth = {1, {2,3}};
	tree tr;
	gt::encode(truth, tr);

	pval_gval pair = {0,{0,0}};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(pair.second.first == 2);
	BOOST_TEST(pair.second.second == 3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PConPVal)

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<pcon_pval>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<pcon_pval>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	pcon_pval pair = {v123, 4};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	uint n = 0;
	vec v;
	gt::decode(tr, n);
	gt::decode(tr, v);

	BOOST_TEST(v == v123, tt::per_element());
	BOOST_TEST(n == 4);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	pcon_pval truth = {v123,4};
	tree tr;
	gt::encode(truth, tr);

	pcon_pval pair = {{},0};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == v123, tt::per_element());
	BOOST_TEST(pair.second == 4);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(GValPVal)

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<gval_pval>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<gval_pval>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gval_pval pair = {{1, 2}, 3};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	BOOST_REQUIRE(tr.child_count() == 1);

	pval_pval ab = {0,0};
	uint c = 0;

	gt::decode(tr, c);
	gt::decode(tr.child(0), ab);

	BOOST_TEST(ab.first == 1);
	BOOST_TEST(ab.second == 2);
	BOOST_TEST(c == 3);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gval_pval truth = {{1,2},3};
	tree tr;
	gt::encode(truth, tr);

	gval_pval pair = {{0,0},0};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first.first == 1);
	BOOST_TEST(pair.first.second == 2);
	BOOST_TEST(pair.second == 3);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(PConPCon)

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<pcon_pcon>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<pcon_pcon>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	pcon_pcon pair = {v123, v456};

	tree tr;
	BOOST_TEST(!gt::encode(pair, tr));

	BOOST_REQUIRE(tr.child_count() == 2);
	vec u, v;
	gt::decode(tr.child(0), u);
	gt::decode(tr.child(1), v);

	BOOST_TEST(u == v123, tt::per_element());
	BOOST_TEST(v == v456, tt::per_element());
}

BOOST_AUTO_TEST_CASE(Decode)
{
	pcon_pcon truth = {v123,v456};
	tree tr;
	gt::encode(truth, tr);

	pcon_pcon pair = {{},{}};
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(pair.first == v123, tt::per_element());
	BOOST_TEST(pair.second == v456, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END()
