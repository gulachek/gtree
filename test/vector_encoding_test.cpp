#define BOOST_TEST_MODULE VectorEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>
#include <variant>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	gt::mutable_tree tr;

	std::vector<std::uint8_t> vec = {1, 2, 3}; // something else
	BOOST_TEST(!gt::decode(tr, vec));

	BOOST_TEST(vec.empty());
}

BOOST_AUTO_TEST_CASE(DecodeNonEmpty)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } },
		gt::mutable_tree{ { 'w', 'o', 'r', 'l', 'd' } }
	}};

	std::vector<std::string> vec;
	BOOST_TEST(!gt::decode(tr, vec));

	std::vector<std::string> expect = {"hello", "world"};
	BOOST_TEST(vec == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(FailToDecodeElem)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ { 0, 1, 2, 3, 4 } },
		gt::mutable_tree{ { 0, 1, 2, 3, 4 } }
	}};

	std::vector<std::uint32_t> vec;
	BOOST_TEST(gt::decode(tr, vec));
}

BOOST_AUTO_TEST_CASE(EncodeNonEmpty)
{
	std::vector<std::string> truth = {"hello", "world"};
	std::vector<std::string> vec;

	gt::mutable_tree tr;
	BOOST_TEST(!gt::encode(truth, tr));
	gt::decode(tr, vec);

	BOOST_TEST(vec == truth, tt::per_element());
}

