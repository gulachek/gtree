#define BOOST_TEST_MODULE VariantEncodingTest
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

BOOST_AUTO_TEST_CASE(UsesValue)
{
	using my_var = std::variant<int, float>;
	BOOST_TEST(gt::uses_value<my_var>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	using my_var = std::variant<int, float>;
	BOOST_TEST(gt::uses_children<my_var>::value);
}

typedef std::variant<std::size_t, std::string, std::vector<std::string>> UintStrVec; 

BOOST_AUTO_TEST_CASE(DecodeValueType)
{
	gt::mutable_tree tr{ {0}, { gt::mutable_tree{ { 25 } } } };

	UintStrVec var;
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 0);

	auto val = std::get<std::size_t>(var);
	BOOST_TEST(val == 25);
}

BOOST_AUTO_TEST_CASE(DecodeSecondValueType)
{
	gt::mutable_tree tr{ {1}, { gt::mutable_tree{ { 'a', 'b', 'c' } } } };

	UintStrVec var;
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 1);

	auto val = std::get<std::string>(var);
	BOOST_TEST(val == "abc");
}

BOOST_AUTO_TEST_CASE(DecodeValuelessType)
{
	gt::mutable_tree tr{ {2}, {
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } },
		gt::mutable_tree{ { 'w', 'o', 'r', 'l', 'd' } }
	}};

	UintStrVec var;
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 2);

	auto val = std::get<std::vector<std::string>>(var);
	std::vector<std::string> expect = { "hello", "world" };

	BOOST_TEST(val == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(EncodeFirstType)
{
	UintStrVec truth = 32LU, var;

	gt::mutable_tree tr;
	gt::encode(truth, tr);
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 0);

	auto val = std::get<std::size_t>(var);
	BOOST_TEST(val == 32);
}

BOOST_AUTO_TEST_CASE(EncodeSecondType)
{
	UintStrVec truth = "hello", var;

	gt::mutable_tree tr;
	gt::encode(truth, tr);
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 1);

	auto val = std::get<std::string>(var);
	BOOST_TEST(val == "hello");
}

BOOST_AUTO_TEST_CASE(EncodeThirdType)
{
	std::vector<std::string> expect = {"hello", "world"};
	UintStrVec truth = expect, var;

	gt::mutable_tree tr;
	gt::encode(truth, tr);
	gt::decode(tr, var);

	BOOST_REQUIRE(var.index() == 2);

	auto val = std::get<std::vector<std::string>>(var);

	BOOST_TEST(val == expect, tt::per_element());
}

