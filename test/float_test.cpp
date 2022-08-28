#define BOOST_TEST_MODULE FloatTest
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/float.hpp"
#include <vector>
#include <sstream>
#include <cstdint>
#include <cmath>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeZero)
{
	vec val = { 0, 0, 0, 0 };
	gt::tree tr{ val };
	float f = 1;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == 0.0f);
	BOOST_TEST(!std::signbit(f));
}

BOOST_AUTO_TEST_CASE(DecodeNegativeZero)
{
	vec val = { 0, 0, 0, 0x80 };
	gt::tree tr{ val };
	float f = 1;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == 0.0f);
	BOOST_TEST(std::signbit(f));
}

BOOST_AUTO_TEST_CASE(DecodeEmptyIsError)
{
	gt::tree tr;
	float f = 1;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeOne)
{
	vec val = { 0, 0, 0x80, 0x3f };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == 1.0f);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeOne)
{
	vec val = { 0, 0, 0x80, 0xbf };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == -1.0f);
}

BOOST_AUTO_TEST_CASE(DecodeSubnormal)
{
	vec val = { 0, 0, 0x40, 0x00 };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == 5.87747175411e-39f);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeSubnormal)
{
	vec val = { 0, 0, 0x40, 0x80 };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == -5.87747175411e-39f);
}

BOOST_AUTO_TEST_CASE(DecodeInf)
{
	vec val = { 0, 0, 0x80, 0x7f };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(std::isinf(f));
	BOOST_TEST(f > 0);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeInf)
{
	vec val = { 0, 0, 0x80, 0xff };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(std::isinf(f));
	BOOST_TEST(f < 0);
}

BOOST_AUTO_TEST_CASE(DecodeNan)
{
	vec val = { 0, 0, 0xc0, 0x7f };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(std::isnan(f));
}

BOOST_AUTO_TEST_CASE(DecodeNormal)
{
	vec val = { 0x11, 0x11, 0x80, 0x3f };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == 1.00052082539f);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeNormal)
{
	vec val = { 0x11, 0x11, 0x80, 0xbf };
	gt::tree tr{ val };
	float f = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &f);

	BOOST_TEST(!err);
	BOOST_TEST(f == -1.00052082539f);
}

BOOST_AUTO_TEST_CASE(EncodeInf)
{
	float f = INFINITY;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x80);
	BOOST_TEST(val[3] == 0x7f);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeInf)
{
	float f = -INFINITY;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x80);
	BOOST_TEST(val[3] == 0xff);
}

BOOST_AUTO_TEST_CASE(EncodeNan)
{
	float f = std::nan("");

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x01);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x80);
	BOOST_TEST(val[3] == 0x7f);
}

BOOST_AUTO_TEST_CASE(EncodeNormal)
{
	float f = 1.00052082539f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x11);
	BOOST_TEST(val[1] == 0x11);
	BOOST_TEST(val[2] == 0x80);
	BOOST_TEST(val[3] == 0x3f);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeNormal)
{
	float f = -1.00052082539f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x11);
	BOOST_TEST(val[1] == 0x11);
	BOOST_TEST(val[2] == 0x80);
	BOOST_TEST(val[3] == 0xbf);
}

BOOST_AUTO_TEST_CASE(EncodeSubnormal)
{
	float f = 5.87747175411e-39f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x40);
	BOOST_TEST(val[3] == 0x00);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeSubnormal)
{
	float f = -5.87747175411e-39f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x40);
	BOOST_TEST(val[3] == 0x80);
}

BOOST_AUTO_TEST_CASE(EncodeZero)
{
	float f = 0.0f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x00);
	BOOST_TEST(val[3] == 0x00);
}

BOOST_AUTO_TEST_CASE(EncodeNegativeZero)
{
	float f = -0.0f;

	gt::tree tr;
	std::stringstream ss;

	gt::write(ss, f);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.child_count() == 0);

	auto val = tr.value();
	BOOST_TEST(val.size() == 4);
	BOOST_TEST(val[0] == 0x00);
	BOOST_TEST(val[1] == 0x00);
	BOOST_TEST(val[2] == 0x00);
	BOOST_TEST(val[3] == 0x80);
}
