#define BOOST_TEST_MODULE VectorEncodingTest
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

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<std::vector<int>>::value);
	BOOST_TEST(!gt::uses_value<std::vector<std::string>>::value);

	std::variant<int, float, std::vector<std::string>> var;
	bool test = !gt::uses_value<
			typename std::variant_alternative<2, decltype(var)>::type
			>::value;

	BOOST_TEST(test);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<std::vector<int>>::value);
}

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	gt::mutable_tree tr;

	std::vector<std::uint8_t> vec = {1, 2, 3}; // something else
	gt::decode(tr, vec);

	BOOST_TEST(vec.empty());
}

BOOST_AUTO_TEST_CASE(DecodeNonEmpty)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } },
		gt::mutable_tree{ { 'w', 'o', 'r', 'l', 'd' } }
	}};

	std::vector<std::string> vec;
	gt::decode(tr, vec);

	std::vector<std::string> expect = {"hello", "world"};
	BOOST_TEST(vec == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(EncodeNonEmpty)
{
	std::vector<std::string> truth = {"hello", "world"};
	std::vector<std::string> vec;

	gt::mutable_tree tr;
	gt::encode(truth, tr);
	gt::decode(tr, vec);

	BOOST_TEST(vec == truth, tt::per_element());
}

