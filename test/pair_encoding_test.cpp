#define BOOST_TEST_MODULE PairEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <optional>
#include <cstdint>

/* Pair <A,B>
 * if A is pure value:
 * 	if B is pure container:
 * 		{ A | ...B }
 * 	else
 * 		{ A | B }
 * else if B is pure value:
 * 	if A is pure container:
 * 		{ B | ...A }
 * 	else:
 * 		{ B | A }
 * else:
 * 	{ A B }
 */

namespace gt = gulachek::gtree;

using size = std::size_t;
using osize = std::optional<size>;

BOOST_AUTO_TEST_CASE(UsesValue)
{
	using pair = std::pair<size, osize>;
	BOOST_TEST(gt::uses_value< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	using pair = std::pair<size, osize>;
	BOOST_TEST(gt::uses_children< pair >::value);
}

BOOST_AUTO_TEST_SUITE(ValueContainer)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 1 }, {
		gt::mutable_tree{ {2} }
	}};

	std::pair<size, osize> pair;
	gt::decode(tr, pair);

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(*(pair.second) == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 1 }, {
		gt::mutable_tree{ {2} }
	}}, result;

	std::pair<size, osize> pair = {1, 2};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // ValueContainer

BOOST_AUTO_TEST_SUITE(ValueGeneral)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 1 }, {
		gt::mutable_tree{ {2} }
	}};

	std::pair<size, size> pair;
	gt::decode(tr, pair);

	BOOST_TEST(pair.first == 1);
	BOOST_TEST(pair.second == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 1 }, {
		gt::mutable_tree{ {2} }
	}}, result;

	std::pair<size, size> pair = {1, 2};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // ValueGeneral

BOOST_AUTO_TEST_SUITE(ContainerValue)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 2 }, {
		gt::mutable_tree{ {1} }
	}};

	std::pair<osize, size> pair;
	gt::decode(tr, pair);

	BOOST_TEST(*(pair.first) == 1);
	BOOST_TEST(pair.second == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 2 }, {
		gt::mutable_tree{ {1} }
	}}, result;

	std::pair<osize, size> pair = {1, 2};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // ContainerValue

struct Foo { size m; size n; };

template <typename MutableTree>
void encode(const Foo &foo, MutableTree &tree)
{
	tree.child_count(1);
	encode(foo.m, tree);
	encode(foo.n, tree.child(0));
}

template <typename Tree>
void decode(const Tree &tree, Foo &foo)
{
	decode(tree, foo.m);
	decode(tree.child(0), foo.n);
}

BOOST_AUTO_TEST_SUITE(GeneralValue)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 3 }, {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	}
	}};

	std::pair<Foo, size> pair;
	gt::decode(tr, pair);

	BOOST_TEST(pair.first.m == 1);
	BOOST_TEST(pair.first.n == 2);
	BOOST_TEST(pair.second == 3);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 3 }, {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	}
	}}, result;

	std::pair<Foo, size> pair = {{1, 2}, 3};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // GeneralValue

BOOST_AUTO_TEST_SUITE(GeneralGeneral)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	},
		gt::mutable_tree{ {3}, {
			gt::mutable_tree{ {4} }
		}	}
	}};

	std::pair<Foo, Foo> pair;
	gt::decode(tr, pair);

	BOOST_TEST(pair.first.m == 1);
	BOOST_TEST(pair.first.n == 2);
	BOOST_TEST(pair.second.m == 3);
	BOOST_TEST(pair.second.n == 4);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	},
		gt::mutable_tree{ {3}, {
			gt::mutable_tree{ {4} }
		}	}
	}}, result;

	std::pair<Foo, Foo> pair = {{1, 2}, {3, 4}};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // GeneralGeneral
