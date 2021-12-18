#define BOOST_TEST_MODULE ClassEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/class.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <variant>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_SUITE(Unsigned)

struct foo
{
	using gtree_encoding = unsigned int;

	gtree_encoding n;

	gt::error gtree_encode(unsigned int &n) const
	{
		if (this->n == 42)
			return "42";

		n = this->n;
		return {};
	}

	gt::error gtree_decode(unsigned int n)
	{
		if (n == 42)
			return "42";

		this->n = n;
		return {};
	}
};

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {10} };

	foo foo;

	BOOST_TEST(!gt::decode(tr, foo));
	BOOST_TEST(foo.n == 10);
}

BOOST_AUTO_TEST_CASE(DecodeCanError)
{
	gt::mutable_tree tr{ {42} };

	foo foo;

	BOOST_TEST(gt::decode(tr, foo));
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {10} }, result;

	foo foo{10};

	BOOST_TEST(!gt::encode(foo, result));
	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(EncodeCanError)
{
	gt::mutable_tree result;

	foo foo{42};

	BOOST_TEST(gt::encode(foo, result));
}

BOOST_AUTO_TEST_SUITE_END() // Unsigned

BOOST_AUTO_TEST_SUITE(ManualEncoding)

class val
{
	public:
		val(int n = 0) : _n{n} {}

		using gtree_encoding = gt::manual_encoding;

		template <typename MutableTree>
		gt::error gtree_encode(MutableTree &tr) const
		{
			return gt::encode((std::uint64_t)_n, tr);
		}

		template <typename Tree>
		gt::error gtree_decode(Tree &&tr)
		{
			std::uint16_t n;
			if (auto err = gt::decode(std::forward<Tree>(tr), n))
				return err;

			_n = n;
			return {};
		}

		int n() const { return _n; }

	private:
		int _n;
};

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {10} }, result;

	val v{10};
	BOOST_TEST(!gt::encode(v, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {10} };

	val v;
	BOOST_TEST(!gt::decode(tr, v));

	BOOST_TEST(v.n() == 10);
}

BOOST_AUTO_TEST_SUITE_END() // ManualEncoding
