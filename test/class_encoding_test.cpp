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

BOOST_AUTO_TEST_SUITE(ValueEncoding)

class val
{
	public:
		val(int n = 0) : _n{n} {}

		using gtree_encoding = gt::value_encoding;

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

BOOST_AUTO_TEST_SUITE_END() // ValueEncoding

BOOST_AUTO_TEST_SUITE(ContainerEncoding)

class cont
{
	public:
		cont(std::vector<std::uint16_t> v = {}) : _v{v} {}

		using gtree_encoding = gt::container_encoding;

		template <typename MutableTree>
		gt::error gtree_encode(MutableTree &tr) const
		{
			return gt::encode(_v, tr);
		}

		template <typename Tree>
		gt::error gtree_decode(Tree &&tr)
		{
			return gt::decode(std::forward<Tree>(tr), _v);
		}

		std::vector<std::uint16_t> v() const { return _v; }

	private:
		std::vector<std::uint16_t> _v;
};

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{{
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} },
		gt::mutable_tree{ {3} }
	}}, result;

	std::vector<std::uint16_t> v = {1, 2, 3};
	cont c{v};
	BOOST_TEST(!gt::encode(c, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} },
		gt::mutable_tree{ {3} }
	}};

	cont c;
	BOOST_TEST(!gt::decode(tr, c));

	std::vector<std::uint16_t> expect = {1, 2, 3};
	BOOST_TEST(c.v() == expect, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END() // ContainerEncoding

BOOST_AUTO_TEST_SUITE(HybridEncoding)

class hybrid
{
	public:
		hybrid(
				std::uint16_t n = 0,
				std::vector<std::uint16_t> v = {}
				) : _n{n}, _v{v} {}

		using gtree_encoding = gt::hybrid_encoding;

		template <typename MutableTree>
		gt::error gtree_encode(MutableTree &tr) const
		{
			if (auto err = gt::encode(_n, tr))
				return err;

			if (auto err = gt::encode(_v, tr))
				return err;

			return {};
		}

		template <typename Tree>
		gt::error gtree_decode(Tree &&tr)
		{
			if (auto err = gt::decode(std::forward<Tree>(tr), _n))
				return err;

			if (auto err = gt::decode(std::forward<Tree>(tr), _v))
				return err;

			return {};
		}

		std::uint16_t n() const { return _n; }
		std::vector<std::uint16_t> v() const { return _v; }

	private:
		std::uint16_t _n;
		std::vector<std::uint16_t> _v;
};

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{{1}, {
		gt::mutable_tree{ {2} },
		gt::mutable_tree{ {3} },
		gt::mutable_tree{ {4} }
	}}, result;

	std::vector<std::uint16_t> v = {2, 3, 4};
	hybrid h{1, v};
	BOOST_TEST(!gt::encode(h, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{{1}, {
		gt::mutable_tree{ {2} },
		gt::mutable_tree{ {3} },
		gt::mutable_tree{ {4} }
	}};

	hybrid h;
	BOOST_TEST(!gt::decode(tr, h));

	std::vector<std::uint16_t> expect = {2, 3, 4};
	BOOST_TEST(h.n() == 1);
	BOOST_TEST(h.v() == expect, tt::per_element());
}

BOOST_AUTO_TEST_SUITE_END() // ContainerEncoding
