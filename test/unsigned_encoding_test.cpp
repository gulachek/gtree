#define BOOST_TEST_MODULE UnsignedEncodingTest
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

typedef std::tuple<
	std::uint8_t,
	std::uint16_t,
	std::uint32_t,
	std::uint64_t
	> unsigned_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(UsesValue, T, unsigned_types)
{
	BOOST_TEST(gt::uses_value<T>::value);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(NoChildren, T, unsigned_types)
{
	BOOST_TEST(!gt::uses_children<T>::value);
}

BOOST_AUTO_TEST_CASE(DecodeSingleByte)
{
	gt::mutable_tree in{ {25} };

	std::uint8_t n = 0;
	gt::decode(in, n);

	BOOST_TEST(n == 25);
}

BOOST_AUTO_TEST_CASE(EncodeSingleByte)
{
	gt::mutable_tree tr;
	gt::encode((std::uint8_t)25U, tr);

	std::size_t n = 0;
	gt::decode(tr, n);

	BOOST_TEST(n == 25);
}

BOOST_AUTO_TEST_CASE(DecodeMultiByte)
{
	gt::mutable_tree in{ {0, 0, 3} };

	std::size_t n = 0;
	gt::decode(in, n);

	BOOST_TEST(n == (3 << 16));
}

BOOST_AUTO_TEST_CASE(EncodeMultiByte)
{
	auto truth = 547025489LU;

	gt::mutable_tree tr;
	gt::encode(truth, tr);

	std::size_t n = 0;
	gt::decode(tr, n);

	BOOST_TEST(n == truth);
}
