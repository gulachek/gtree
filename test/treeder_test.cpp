#define BOOST_TEST_MODULE ItreemTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/itreem.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <sstream>

namespace gt = gulachek::gtree;

auto vec_stream(const std::vector<std::uint8_t> &vec)
{
	std::stringstream _ss;
	_ss.write((char *)vec.data(), vec.size());
	return _ss;
}

BOOST_AUTO_TEST_CASE(Basic)
{
	// A{B,C}
	auto stream = vec_stream({
		7, // size of node block
		3, // number of nodes
		1, 2,
		1, 0,
		1, 0,
		'a', 'b', 'c'
	});

	gt::itreem treeder{stream};

	gt::tree t;
	bool tree_was_read = treeder.read(&t);

	BOOST_TEST(tree_was_read);
	std::string a; gt::decode(t, a);
	BOOST_TEST(a == "a");
	BOOST_TEST(t.child_count() == 2);

	auto tb = t.child(0);
	auto tc = t.child(1);

	std::string b; gt::decode(tb, b);
	BOOST_TEST(b == "b");
	BOOST_TEST(tb.child_count() == 0);

	std::string c; gt::decode(tc, c);
	BOOST_TEST(c == "c");
	BOOST_TEST(tc.child_count() == 0);
}
