#define BOOST_TEST_MODULE FusionEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/fusion.hpp"
#include "gulachek/gtree/encoding/variant.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/optional.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/vector.hpp"

#include <boost/fusion/adapted/adt/adapt_adt.hpp>
#include <boost/fusion/include/adapt_adt.hpp>

#include <optional>
#include <cstdint>

namespace gt = gulachek::gtree;
namespace fusion = boost::fusion;
using fusion::at_c;

typedef std::variant<std::string, std::uint32_t> test_general;
typedef std::uint32_t test_value;
typedef std::optional<std::uint32_t> test_container;

typedef std::tuple<
	test_general,
	test_value,
	test_container
	> test_types;

BOOST_AUTO_TEST_SUITE(Tuple)

BOOST_AUTO_TEST_SUITE(SingleElement)

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {}, {gt::mutable_tree{ { 25 } }} }, result;
	fusion::vector<test_value> val{25};
	BOOST_TEST(!gt::encode(val, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {}, {gt::mutable_tree{ { 25 }}} };
	fusion::vector<test_value> val;

	BOOST_TEST(!gt::decode(tr, val));

	auto n = fusion::at_c<0>(val);

	BOOST_TEST(n == 25);
}

BOOST_AUTO_TEST_SUITE_END() // SingleElement

BOOST_AUTO_TEST_SUITE(Pair)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {}, {
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} }
	}};

	fusion::vector<test_value, test_value> pair;
	BOOST_TEST(!gt::decode(tr, pair));

	BOOST_TEST(at_c<0>(pair) == 1);
	BOOST_TEST(at_c<1>(pair) == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {}, {
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} }
	}}, result;

	fusion::vector<test_value, test_value> pair{1, 2};
	BOOST_TEST(!gt::encode(pair, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // Pair

BOOST_AUTO_TEST_SUITE(ThreePlus)

typedef fusion::vector<
	test_value,
	test_value,
	test_value
	> three_vals;

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {
		gt::mutable_tree{ { 1 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}}, result;

	three_vals vals{1, 2, 3};
	BOOST_TEST(!gt::encode(vals, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {
		gt::mutable_tree{ { 1 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}};

	three_vals vals;
	BOOST_TEST(!gt::decode(tr, vals));

	BOOST_CHECK(at_c<0>(vals) == 1);
	BOOST_CHECK(at_c<1>(vals) == 2);
	BOOST_CHECK(at_c<2>(vals) == 3);
}

BOOST_AUTO_TEST_SUITE_END() // ThreePlus

BOOST_AUTO_TEST_SUITE_END() // Tuple

class single
{
	public:
		std::uint32_t n() const { return _n; }
		void n(std::uint32_t n) { _n = n; }

	private:
		std::uint32_t _n;
};

BOOST_FUSION_ADAPT_ADT(::single,
		(obj.n(), obj.n(val))
		);

class triple
{
	public:
		std::uint32_t first() const { return _f; }
		void first(std::uint32_t f) { _f = f; }

		std::uint32_t second() const { return _s; }
		void second(std::uint32_t s) { _s = s; }

		std::uint32_t third() const { return _t; }
		void third(std::uint32_t t) { _t = t; }

	private:
		std::uint32_t _f;
		std::uint32_t _s;
		std::uint32_t _t;
};

BOOST_FUSION_ADAPT_ADT(::triple,
		(obj.first(), obj.first(val))
		(obj.second(), obj.second(val))
		(obj.third(), obj.third(val))
		);

class my_pair
{
	public:
		std::uint32_t first() const { return _f; }
		void first(std::uint32_t f) { _f = f; }

		std::uint32_t second() const { return _s; }
		void second(std::uint32_t s) { _s = s; }


	private:
		std::uint32_t _f;
		std::uint32_t _s;
};

BOOST_FUSION_ADAPT_ADT(::my_pair,
		(obj.first(), obj.first(val))
		(obj.second(), obj.second(val))
		);

BOOST_AUTO_TEST_SUITE(ADT)

BOOST_AUTO_TEST_SUITE(SingleElement)

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {}, {gt::mutable_tree{ { 25 }}} }, result;
	single val;
	val.n(25);
	BOOST_TEST(!gt::encode(val, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {}, {gt::mutable_tree{ { 25 }}} };
	single val;

	BOOST_TEST(!gt::decode(tr, val));

	BOOST_TEST(val.n() == 25);
}

BOOST_AUTO_TEST_CASE(NotEnoughChildrenFails)
{
	gt::mutable_tree tr;
	single val;

	BOOST_TEST(gt::decode(tr, val));
}

BOOST_AUTO_TEST_CASE(DecodeOfChildFails)
{
	gt::mutable_tree tr{ {}, {gt::mutable_tree{{ 25, 2, 3, 4, 5 } }}};
	single val;

	BOOST_TEST(gt::decode(tr, val));
}

BOOST_AUTO_TEST_SUITE_END() // SingleElement

BOOST_AUTO_TEST_SUITE(Pair)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {}, {
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} }
	}};

	my_pair p;
	BOOST_TEST(!gt::decode(tr, p));

	BOOST_TEST(p.first() == 1);
	BOOST_TEST(p.second() == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {}, {
		gt::mutable_tree{ {1} },
		gt::mutable_tree{ {2} }
	}}, result;

	my_pair p;
	p.first(1);
	p.second(2);
	BOOST_TEST(!gt::encode(p, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // Pair

BOOST_AUTO_TEST_SUITE(ThreePlus)

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {
		gt::mutable_tree{ { 1 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}}, result;

	triple vals;
	vals.first(1);
	vals.second(2);
	vals.third(3);
	BOOST_TEST(!gt::encode(vals, result));

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ {
		gt::mutable_tree{ { 1 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}};

	triple vals;
	BOOST_TEST(!gt::decode(tr, vals));

	BOOST_CHECK(vals.first() == 1);
	BOOST_CHECK(vals.second() == 2);
	BOOST_CHECK(vals.third() == 3);
}

BOOST_AUTO_TEST_CASE(DecodeFails)
{
	gt::mutable_tree tr{ {
		gt::mutable_tree{ { 1, 2, 3, 4, 5 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}};

	triple vals;
	BOOST_TEST(gt::decode(tr, vals));
}

BOOST_AUTO_TEST_SUITE_END() // ThreePlus

BOOST_AUTO_TEST_SUITE_END() // ADT
