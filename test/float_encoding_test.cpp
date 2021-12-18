#define BOOST_TEST_MODULE FloatEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/float.hpp"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(LessThan4BytesError)
{
	gt::mutable_tree in{ {0, 0, 1} };
	float f;
	BOOST_TEST(gt::decode(in, f));
}

BOOST_AUTO_TEST_CASE(MoreThan4BytesError)
{
	gt::mutable_tree in{ {0, 0, 0, 0, 1} };
	float f;
	BOOST_TEST(gt::decode(in, f));
}

BOOST_AUTO_TEST_CASE(DecodePositiveZero)
{
	gt::mutable_tree in{ {0, 0, 0, 0} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == 0.0f);
	BOOST_TEST(!std::signbit(f));
}

BOOST_AUTO_TEST_CASE(DecodeNegativeZero)
{
	gt::mutable_tree in{ {0, 0, 0, 0x80} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == 0.0f);
	BOOST_TEST(std::signbit(f));
}

BOOST_AUTO_TEST_CASE(DecodePositiveSubnormal)
{
	gt::mutable_tree in{ {0, 0, 0x20, 0x00} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == 2.938735877055719e-39);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeSubnormal)
{
	gt::mutable_tree in{ {0, 0, 0x20, 0x80} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == -2.938735877055719e-39);
}

BOOST_AUTO_TEST_CASE(DecodePositiveNormal)
{
	gt::mutable_tree in{ {0, 0, 0x20, 0x3e} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == 0.15625f);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeNormal)
{
	gt::mutable_tree in{ {0, 0, 0x20, 0xbe} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(f == -0.15625f);
}

BOOST_AUTO_TEST_CASE(DecodePositiveNan)
{
	gt::mutable_tree in{ {0, 0, 0xa0, 0x7f} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(std::isnan(f));
}

BOOST_AUTO_TEST_CASE(DecodeNegativeNan)
{
	gt::mutable_tree in{ {0, 0, 0xa0, 0xff} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(std::isnan(f));
}

BOOST_AUTO_TEST_CASE(DecodePositiveInf)
{
	gt::mutable_tree in{ {0, 0, 0x80, 0x7f} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(std::isinf(f));
	BOOST_TEST(f > 0.0f);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeInf)
{
	gt::mutable_tree in{ {0, 0, 0x80, 0xff} };

	float f = 0.0f;
	BOOST_TEST(!gt::decode(in, f));

	BOOST_TEST(std::isinf(f));
	BOOST_TEST(f < 0.0f);
}

BOOST_AUTO_TEST_CASE(EncodePositiveZero)
{
	gt::mutable_tree truth{ {0, 0, 0, 0} }, out;

	float f = 0.0f;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeZero)
{
	gt::mutable_tree truth{ {0, 0, 0, 0x80} }, out;

	float f = -0.0f;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodePositiveSubnormal)
{
	gt::mutable_tree truth{ {0, 0, 0x20, 0x00} }, out;

	float f = 2.938735877055719e-39;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeSubnormal)
{
	gt::mutable_tree truth{ {0, 0, 0x20, 0x80} }, out;

	float f = -2.938735877055719e-39;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodePositiveNormal)
{
	gt::mutable_tree truth{ {0, 0, 0x20, 0x3e} }, out;

	float f = 0.15625f;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeNormal)
{
	gt::mutable_tree truth{ {0, 0, 0x20, 0xbe} }, out;

	float f = -0.15625f;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodeNan)
{
	gt::mutable_tree truth{ {1, 0, 0x80, 0x7f} }, out;

	float f = std::nan("");
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodePositiveInf)
{
	gt::mutable_tree truth{ {0, 0, 0x80, 0x7f} }, out;

	float f = INFINITY;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeInf)
{
	gt::mutable_tree truth{ {0, 0, 0x80, 0xff} }, out;

	float f = -INFINITY;
	BOOST_TEST(!gt::encode(f, out));

	BOOST_CHECK(out == truth);
}
