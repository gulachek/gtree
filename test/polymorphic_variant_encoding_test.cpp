#define BOOST_TEST_MODULE PolymorphicVariantEncodingTest
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

typedef unsigned int uint;

struct Base {
	virtual uint foo() { return 0; }
	virtual uint bar() const { return 0; }
};

struct A : Base {
	typedef uint gtree_encoding;
	uint n = 1;
	A() = default;
	A(uint n) : n{n} {}

	static const uint& gtree_encode(const A &a) { return a.n; }
	static A gtree_decode(uint &&n) { return A{n}; }

	uint foo() override { return n; }
	uint bar() const override { return n; }
};

struct B : Base {
	typedef uint gtree_encoding;
	uint n = 1;
	B() = default;
	B(uint n) : n{n} {}

	static const uint& gtree_encode(const B &a) { return a.n; }
	static B gtree_decode(uint &&n) { return B{n}; }

	uint foo() override { return 2*n; }
	uint bar() const override { return 2*n; }
};

BOOST_AUTO_TEST_CASE(CanAccessAsBasePtr)
{
	gt::polymorphic_variant<Base, A, B> var = A{};

	BOOST_TEST(var->foo() == 1);
}

BOOST_AUTO_TEST_CASE(CanAccessAsConstBasePtr)
{
	const gt::polymorphic_variant<Base, A, B> var = B{};

	BOOST_TEST(var->bar() == 2);
}

BOOST_AUTO_TEST_CASE(CanAccessAsBaseRef)
{
	gt::polymorphic_variant<Base, A, B> var = A{};

	BOOST_TEST((*var).foo() == 1);
}

BOOST_AUTO_TEST_CASE(CanAccessAsConstBaseRef)
{
	const gt::polymorphic_variant<Base, A, B> var = B{};

	BOOST_TEST((*var).bar() == 2);
}

BOOST_AUTO_TEST_CASE(DecodeFirstAlternative)
{
	gt::mutable_tree tr{ {0}, {
		gt::mutable_tree{ {10} } // A{10}
	}};

	gt::polymorphic_variant<Base, A, B> var;
	gt::decode(tr, var);

	BOOST_TEST(var->foo() == 10);
}

BOOST_AUTO_TEST_CASE(DecodeSecondAlternative)
{
	gt::mutable_tree tr{ {1}, {
		gt::mutable_tree{ {10} } // B{10}
	}};

	gt::polymorphic_variant<Base, A, B> var;
	gt::decode(tr, var);

	BOOST_TEST(var->foo() == 20);
}

BOOST_AUTO_TEST_CASE(EncodeFirstAlternative)
{
	gt::mutable_tree expect{ {}, {
		gt::mutable_tree{ {10} } // A{10}
	}}, result;

	gt::polymorphic_variant<Base, A, B> var = A{10};

	gt::encode(var, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(EncodeSecondAlternative)
{
	gt::mutable_tree expect{ {1}, {
		gt::mutable_tree{ {10} } // B{10}
	}}, result;

	gt::polymorphic_variant<Base, A, B> var = B{10};

	gt::encode(var, result);

	BOOST_CHECK(result == expect);
}
