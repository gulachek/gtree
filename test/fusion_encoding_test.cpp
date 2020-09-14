#define BOOST_TEST_MODULE FusionEncodingTest
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

BOOST_AUTO_TEST_SUITE(SingleElement)

BOOST_AUTO_TEST_CASE_TEMPLATE(UsesValueSameAsUnderlying, T, test_types)
{
	auto expect = gt::uses_value<T>::value;
	auto result = gt::uses_value<fusion::vector<T>>::value;
	BOOST_TEST(expect == result);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(UsesChildrenSameAsUnderlying, T, test_types)
{
	auto expect = gt::uses_children<T>::value;
	auto result = gt::uses_children<fusion::vector<T>>::value;
	BOOST_TEST(expect == result);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 25 } }, result;
	fusion::vector<test_value> val{25};
	gt::encode(val, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 25 } };
	fusion::vector<test_value> val;

	gt::decode(tr, val);

	auto n = fusion::at_c<0>(val);

	BOOST_TEST(n == 25);
}

BOOST_AUTO_TEST_SUITE_END() // SingleElement

BOOST_AUTO_TEST_SUITE(Pair)

BOOST_AUTO_TEST_CASE(UsesValueIfValueContainer)
{
	using pair = fusion::vector<test_value, test_container>;
	BOOST_TEST(gt::uses_value< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesValueIfContainerValue)
{
	using pair = fusion::vector<test_value, test_container>;
	BOOST_TEST(gt::uses_value< pair >::value);
}

BOOST_AUTO_TEST_CASE(NoValueIfGeneralGeneral)
{
	using pair = fusion::vector<test_general, test_general>;
	BOOST_TEST(!gt::uses_value< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesChildrenIfValueContainer)
{
	using pair = fusion::vector<test_value, test_container>;
	BOOST_TEST(gt::uses_children< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesChildrenIfContainerValue)
{
	using pair = fusion::vector<test_value, test_container>;
	BOOST_TEST(gt::uses_children< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesChildrenIfGeneralGeneral)
{
	using pair = fusion::vector<test_general, test_general>;
	BOOST_TEST(gt::uses_children< pair >::value);
}

BOOST_AUTO_TEST_CASE(UsesChildrenIfValueValue)
{
	using pair = fusion::vector<test_value, test_value>;
	BOOST_TEST(gt::uses_children< pair >::value);
}

BOOST_AUTO_TEST_SUITE(ValueContainer)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 1 }, {
		gt::mutable_tree{ {2} }
	}};

	fusion::vector<test_value, test_container> pair;
	gt::decode(tr, pair);

	BOOST_TEST(at_c<0>(pair) == 1);
	BOOST_TEST(*(at_c<1>(pair)) == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 1 }, {
		gt::mutable_tree{ {2} }
	}}, result;

	fusion::vector<test_value, test_container> pair{1, 2};
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

	fusion::vector<test_value, test_value> pair;
	gt::decode(tr, pair);

	BOOST_TEST(at_c<0>(pair) == 1);
	BOOST_TEST(at_c<1>(pair) == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 1 }, {
		gt::mutable_tree{ {2} }
	}}, result;

	fusion::vector<test_value, test_value> pair{1, 2};
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

	fusion::vector<test_container, test_value> pair;
	gt::decode(tr, pair);

	BOOST_TEST(*(at_c<0>(pair)) == 1);
	BOOST_TEST(at_c<1>(pair) == 2);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 2 }, {
		gt::mutable_tree{ {1} }
	}}, result;

	fusion::vector<test_container, test_value> pair{1, 2};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // ContainerValue

BOOST_AUTO_TEST_SUITE(GeneralValue)

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{ { 3 }, {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	}
	}};

	fusion::vector<test_general, test_value> pair;
	gt::decode(tr, pair);

	auto n = std::get<std::uint32_t>(at_c<0>(pair));
	BOOST_TEST(n == 2);
	BOOST_TEST(at_c<1>(pair) == 3);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ { 3 }, {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	}
	}}, result;

	fusion::vector<test_general, test_value> pair{2, 3};
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
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {3} }
		}	}
	}};

	fusion::vector<test_general, test_general> pair;
	gt::decode(tr, pair);

	auto m = std::get<std::uint32_t>(at_c<0>(pair));
	auto n = std::get<std::uint32_t>(at_c<1>(pair));
	BOOST_TEST(m == 2);
	BOOST_TEST(n == 3);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {2} }
		}	},
		gt::mutable_tree{ {1}, {
			gt::mutable_tree{ {3} }
		}	}
	}}, result;

	fusion::vector<test_general, test_general> pair{2, 3};
	gt::encode(pair, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_SUITE_END() // GeneralGeneral

BOOST_AUTO_TEST_SUITE_END() // Pair

BOOST_AUTO_TEST_SUITE(ThreePlus)

typedef fusion::vector<
	test_value,
	test_value,
	test_value
	> three_vals;

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<three_vals>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<three_vals>::value);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{ {
		gt::mutable_tree{ { 1 } },
		gt::mutable_tree{ { 2 } },
		gt::mutable_tree{ { 3 } }
	}}, result;

	three_vals vals{1, 2, 3};
	gt::encode(vals, result);

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
	gt::decode(tr, vals);

	BOOST_CHECK(at_c<0>(vals) == 1);
	BOOST_CHECK(at_c<1>(vals) == 2);
	BOOST_CHECK(at_c<2>(vals) == 3);
}

BOOST_AUTO_TEST_SUITE_END() // ThreePlus
