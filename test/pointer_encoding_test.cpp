#define BOOST_TEST_MODULE PointerEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/shared_pointer.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"

#include <memory>
#include <cstdint>
#include <tuple>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DecodeValueType)
{
	std::uint16_t n = 12;
	std::shared_ptr<std::uint16_t> ptr;

	gt::mutable_tree tr;
	gt::encode(n, tr);
	BOOST_TEST(!gt::decode(tr, ptr));

	BOOST_TEST(*ptr == 12);
}

BOOST_AUTO_TEST_CASE(DecodeFails)
{
	std::size_t n = ~0LU;
	std::shared_ptr<std::uint16_t> ptr;

	gt::mutable_tree tr;
	gt::encode(n, tr);
	BOOST_TEST(gt::decode(tr, ptr));
}

BOOST_AUTO_TEST_CASE(EncodeValueType)
{
	auto ptr = std::make_shared<std::uint16_t>(12);
	std::uint16_t n = 0;

	gt::mutable_tree tr;
	BOOST_TEST(!gt::encode(ptr, tr));
	gt::decode(tr, n);

	BOOST_TEST(n == 12);
}
