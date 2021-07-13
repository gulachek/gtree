#define BOOST_TEST_MODULE SignedEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/signed.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace gt = gulachek::gtree;

typedef std::tuple<
	std::int8_t,
	std::int16_t,
	std::int32_t,
	std::int64_t
	> signed_types;

BOOST_AUTO_TEST_CASE_TEMPLATE(UsesValue, T, signed_types)
{
	BOOST_TEST(gt::uses_value<T>::value);
}

BOOST_AUTO_TEST_CASE_TEMPLATE(NoChildren, T, signed_types)
{
	BOOST_TEST(!gt::uses_children<T>::value);
}

BOOST_AUTO_TEST_CASE(DecodeSingleByte)
{
	gt::mutable_tree pos{ {25} };
	gt::mutable_tree neg{ {0xe7} };

	std::int8_t n = 0;

	BOOST_TEST(!gt::decode(pos, n));
	BOOST_TEST(n == 25);

	BOOST_TEST(!gt::decode(neg, n));
	BOOST_TEST(n == -25);
}

BOOST_AUTO_TEST_CASE(DecodeTwoBytes)
{
	// 1234
	gt::mutable_tree pos{ {0xd2, 0x04} };
	gt::mutable_tree neg{ {0x2e, 0xfb} };

	std::int16_t n = 0;

	BOOST_TEST(!gt::decode(pos, n));
	BOOST_TEST(n == 1234);

	BOOST_TEST(!gt::decode(neg, n));
	BOOST_TEST(n == -1234);
}

BOOST_AUTO_TEST_CASE(DecodeThreeBytes)
{
	// 1 million
	gt::mutable_tree pos{ {0x40, 0x42, 0x0f} };
	gt::mutable_tree neg{ {0xc0, 0xbd, 0xf0} };

	std::int32_t n = 0;

	BOOST_TEST(!gt::decode(pos, n));
	BOOST_TEST(n == 1000000);

	BOOST_TEST(!gt::decode(neg, n));
	BOOST_TEST(n == -1000000);
}

BOOST_AUTO_TEST_CASE(DecodeMaxSize)
{
	gt::mutable_tree pos{
		{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};

	gt::mutable_tree neg{
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff }
	};

	std::int64_t n = 0;

	BOOST_TEST(!gt::decode(pos, n));
	BOOST_TEST(n == 1);

	BOOST_TEST(!gt::decode(neg, n));
	BOOST_TEST(n == -1);
}

BOOST_AUTO_TEST_CASE(FailPastMaxSize)
{
	gt::mutable_tree pos{
		{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	};

	std::int64_t n = 0;

	BOOST_TEST(gt::decode(pos, n));
}

BOOST_AUTO_TEST_CASE(EncodeSingleByte)
{
	gt::mutable_tree pos{ {25} };
	gt::mutable_tree neg{ {0xe7} };
	gt::mutable_tree tr;

	std::int8_t n = 25;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == pos);

	n = -25;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == neg);
}

BOOST_AUTO_TEST_CASE(EncodeTwoBytes)
{
	// 1234
	gt::mutable_tree pos{ {0xd2, 0x04} };
	gt::mutable_tree neg{ {0x2e, 0xfb} };
	gt::mutable_tree tr;

	std::int16_t n = 1234;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == pos);

	n = -1234;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == neg);
}

BOOST_AUTO_TEST_CASE(EncodeThreeBytes)
{
	// 1 million
	gt::mutable_tree pos{ {0x40, 0x42, 0x0f} };
	gt::mutable_tree neg{ {0xc0, 0xbd, 0xf0} };
	gt::mutable_tree tr;

	std::int32_t n = 1000000;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == pos);

	n = -1000000;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == neg);
}

BOOST_AUTO_TEST_CASE(EncodeMaxSize)
{
	gt::mutable_tree pos{
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x7f }
	};

	gt::mutable_tree neg{
		{0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80 }
	};
	gt::mutable_tree tr;

	std::int64_t n = 0x7fffffffffffffff;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == pos);

	n = -0x7fffffffffffffff;
	BOOST_TEST(!gt::encode(n, tr));
	BOOST_CHECK(tr == neg);
}
