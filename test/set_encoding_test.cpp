#define BOOST_TEST_MODULE SetEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/set.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <set>
#include <cstdint>
#include <algorithm>
#include <iterator>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	gt::mutable_tree tr;

	std::set<std::uint8_t> set = {1, 2, 3}; // something else
	gt::decode(tr, set);

	BOOST_TEST(set.empty());
}

BOOST_AUTO_TEST_CASE(DecodeNonEmpty)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } },
		gt::mutable_tree{ { 'w', 'o', 'r', 'l', 'd' } }
	}};

	std::set<std::string> set;
	gt::decode(tr, set);

	BOOST_TEST(set.size() == 2);
	BOOST_TEST(set.count("hello"));
	BOOST_TEST(set.count("world"));
}

BOOST_AUTO_TEST_CASE(EncodeNonEmpty)
{
	std::set<std::string> truth = {"hello", "world"};
	std::set<std::string> set;

	gt::mutable_tree tr;
	gt::encode(truth, tr);
	gt::decode(tr, set);

	BOOST_TEST(set.size() == 2);
	BOOST_TEST(set.count("hello"));
	BOOST_TEST(set.count("world"));
}

