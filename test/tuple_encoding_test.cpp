#define BOOST_TEST_MODULE TupleEncodingTest
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

typedef std::tuple<std::size_t, std::string> intstr;

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<intstr>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<intstr>::value);
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{ { 25 } },
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } }
	}};

	intstr val;
	gt::decode(tr, val);

	BOOST_TEST(std::get<0>(val) == 25);
	BOOST_TEST(std::get<1>(val) == "hello");
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree answer{{
		gt::mutable_tree{ { 25 } },
		gt::mutable_tree{ { 'h', 'e', 'l', 'l', 'o' } }
	}}, result;

	intstr val{25, "hello"};

	gt::encode(val, result);

	BOOST_CHECK(result == answer);
}
