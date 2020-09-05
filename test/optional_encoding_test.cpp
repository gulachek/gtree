#define BOOST_TEST_MODULE OptionalEncodingTest
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

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<std::optional<int>>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<std::optional<bool>>::value);
}

BOOST_AUTO_TEST_CASE(DecodeNullopt)
{
	gt::mutable_tree tr;

	std::optional<std::size_t> n = 1;
	gt::decode(tr, n);

	BOOST_TEST(!n.has_value());
}

BOOST_AUTO_TEST_CASE(EncodeNullopt)
{
	std::optional<std::size_t> in, out = 3;

	gt::mutable_tree tr;
	gt::encode(in, tr);
	gt::decode(tr, out);

	BOOST_TEST(!out.has_value());
}

BOOST_AUTO_TEST_CASE(DecodeValue)
{
	gt::mutable_tree ntree;
	gt::encode(10LU, ntree);
	gt::mutable_tree tr{{}, { ntree } };

	std::optional<std::size_t> n;
	gt::decode(tr, n);

	BOOST_TEST(*n == 10);
}

BOOST_AUTO_TEST_CASE(EncodeValue)
{
	std::optional<std::string> in = "hello", out;

	gt::mutable_tree tr;
	gt::encode(in, tr);
	gt::decode(tr, out);

	BOOST_TEST(*in == *out);
}
