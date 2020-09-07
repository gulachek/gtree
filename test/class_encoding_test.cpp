#define BOOST_TEST_MODULE ClassEncodingTest
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

BOOST_AUTO_TEST_SUITE(Unsigned)

struct Foo
{
	using gtree_encoding = unsigned int;

	gtree_encoding n;

	static const gtree_encoding& gtree_encode(const Foo &foo)
	{
		return foo.n;
	}

	static Foo gtree_decode(gtree_encoding &&n)
	{
		return Foo{n};
	}
};

BOOST_AUTO_TEST_CASE(TypeTraitCopiesUsesValue)
{
	struct Container {
		using gtree_encoding = std::vector<std::size_t>;
	};

	struct Value {
		using gtree_encoding = std::size_t;
	};

	BOOST_TEST(gt::uses_value<Value>::value);
	BOOST_TEST(!gt::uses_value<Container>::value);
}

BOOST_AUTO_TEST_CASE(TypeTraitCopiesUsesChildren)
{
	struct Container {
		using gtree_encoding = std::vector<std::size_t>;
	};

	struct Value {
		using gtree_encoding = std::size_t;
	};

	BOOST_TEST(!gt::uses_children<Value>::value);
	BOOST_TEST(gt::uses_children<Container>::value);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {10} };

	Foo foo;
	gt::decode(tr, foo);

	BOOST_TEST(foo.n == 10);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {10} }, result;

	Foo foo{10};
	gt::encode(foo, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // Unsigned
