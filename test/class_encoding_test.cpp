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

	void gtree_encode(unsigned int &n) const
	{
		n = this->n;
	}

	void gtree_decode(unsigned int n)
	{
		this->n = n;
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

BOOST_AUTO_TEST_SUITE(ValueEncoding)

class val
{
	public:
		val(int n = 0) : _n{n} {}

		using gtree_encoding = gt::value_encoding;

		template <typename MutableTree>
		void gtree_encode(MutableTree &tr) const
		{
			gt::encode((std::uint64_t)_n, tr);
		}

		template <typename Tree>
		void gtree_decode(const Tree &tr)
		{
			std::uint16_t n;
			gt::decode(tr, n);
			_n = n;
		}

		int n() const { return _n; }

	private:
		int _n;
};

BOOST_AUTO_TEST_CASE(UsesValue)
{
	BOOST_TEST(gt::uses_value<val>::value);
}

BOOST_AUTO_TEST_CASE(NoChildren)
{
	BOOST_TEST(!gt::uses_children<val>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {10} }, result;

	val v{10};
	gt::encode(v, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {10} };

	val v;
	gt::decode(tr, v);

	BOOST_TEST(v.n() == 10);
}

BOOST_AUTO_TEST_SUITE_END() // ValueEncoding
