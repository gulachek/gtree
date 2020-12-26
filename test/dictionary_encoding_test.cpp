#define BOOST_TEST_MODULE DictionaryEncodingTest
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

GULACHEK_GTREE_DEFINE_DICT(
		(test),
		foo,
		(std::string, str)
		(std::vector<std::size_t>, ns)
		)

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<test::foo>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<test::foo>::value);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	std::tuple<std::string, std::string> str{"str", "hello"};
	std::tuple<std::string, std::vector<std::size_t>>
		ns{"ns", {1, 2, 3}};

	gt::mutable_tree tr;
	tr.child_count(2);
	gt::encode(str, tr.child(0));
	gt::encode(ns, tr.child(1));

	test::foo val;
	gt::decode(tr, val);

	BOOST_TEST(val.str == "hello");

	std::vector<std::size_t> expect_ns = {1, 2, 3};
	BOOST_TEST(val.ns == expect_ns, tt::per_element());
}

BOOST_AUTO_TEST_CASE(Encode)
{
	test::foo foo;
	foo.str = "hello";
	foo.ns = {1, 2, 3};

	std::tuple<std::string, std::string> str;
	std::tuple<std::string, std::vector<std::size_t>> ns;

	gt::mutable_tree tr;
	gt::encode(foo, tr);

	BOOST_TEST(tr.value().size() == 0);
	BOOST_TEST(tr.child_count() == 2);

	gt::decode(tr.child(0), str);
	gt::decode(tr.child(1), ns);

	BOOST_TEST(std::get<0>(str) == "str");
	BOOST_TEST(std::get<1>(str) == foo.str);
	BOOST_TEST(std::get<0>(ns) == "ns");
	BOOST_TEST(std::get<1>(ns) == foo.ns, tt::per_element());
}
