#define BOOST_TEST_MODULE SignedTest
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeEmptyTreeIsZero)
{
	gt::tree tr;
	int n = 1;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 0);
}

BOOST_AUTO_TEST_CASE(DecodeSingleByte)
{
	vec val = { 25 };
	gt::tree tr{ val };
	int n = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 25);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeSingleByte)
{
	vec val = { 0b11100111 };
	gt::tree tr{ val };
	int n = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == -25);
}

BOOST_AUTO_TEST_CASE(DecodeTwoBytesIsLittleEndian)
{
	vec val = { 1, 1 };
	gt::tree tr{ val };
	int n = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 257);
}

BOOST_AUTO_TEST_CASE(DecodeNegativeTwoBytesIsLittleEndian)
{
	vec val = { 0b11111111, 0b11111110 };
	gt::tree tr{ val };
	int n = 0;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == -257);
}

BOOST_AUTO_TEST_CASE(DecodeLargerWidthThanIntIsError)
{
	vec val = { 0, 0, 0 };
	gt::tree tr{ val };
	std::int16_t n = 1;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &n);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeZeroIsEmptyTree)
{
	vec val = {1};
	gt::tree tr{ val };
	int n = 0;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.value().size() == 0);
	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EncodeSingleByte)
{
	gt::tree tr;
	int n = 25;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);

	auto v = tr.value();
	BOOST_TEST(v.size() == 1);
	BOOST_TEST(v[0] == 25);

	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EncodeSingleByteNegative)
{
	gt::tree tr;
	int n = -25;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);

	auto v = tr.value();
	BOOST_TEST(v.size() == 1);
	BOOST_TEST(v[0] == 0b11100111);

	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EncodeMultiByte)
{
	gt::tree tr;
	int n = 257;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);

	auto v = tr.value();
	BOOST_TEST(v.size() == 2);
	BOOST_TEST(v[0] == 1);
	BOOST_TEST(v[1] == 1);

	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EncodeMultiByteNegative)
{
	gt::tree tr;
	int n = -257;
	std::stringstream ss;

	gt::write(ss, n);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);

	auto v = tr.value();
	BOOST_TEST(v.size() == 2);
	BOOST_TEST(v[0] == 0b11111111);
	BOOST_TEST(v[1] == 0b11111110);

	BOOST_TEST(tr.child_count() == 0);
}
