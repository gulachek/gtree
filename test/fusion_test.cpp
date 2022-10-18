#define BOOST_TEST_MODULE FusionTest
#include <boost/test/unit_test.hpp>

#include "convert.hpp"
#include "saboteur.hpp"

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/fusion.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include <boost/fusion/include/define_struct.hpp>

#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using gulachek::error;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_FUSION_DEFINE_STRUCT(
		(demo), employee,
		(std::string, name)
		(std::uint8_t, age)
		(saboteur, s)
		)

BOOST_AUTO_TEST_CASE(DecodeJoe)
{
	gt::tree tr;
	std::string name = "joe";
	std::uint8_t age = 3;

	tr.child_count(3);
	cvt(name, &tr.child(0));
	cvt(age, &tr.child(1));

	demo::employee joe;
	auto err = cvt(tr, &joe);

	BOOST_TEST(!err);
	BOOST_TEST(joe.name == "joe");
	BOOST_TEST(joe.age == 3);
}

BOOST_AUTO_TEST_CASE(DecodeErrorInMemberIsError)
{
	gt::tree tr;
	std::string name = "joe";
	std::uint8_t age = 3;

	tr.child_count(3);
	cvt(name, &tr.child(0));
	cvt(age, &tr.child(1));
	tr.child(2).value({&age, 1});

	demo::employee joe;
	auto err = cvt(tr, &joe);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(DecodeTooFewElemsIsError)
{
	gt::tree tr;
	std::string name = "joe";
	std::uint8_t age = 3;

	tr.child_count(2);
	cvt(name, &tr.child(0));
	cvt(age, &tr.child(1));

	demo::employee joe;
	auto err = cvt(tr, &joe);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeJoe)
{
	demo::employee joe;
	joe.name = "joe";
	joe.age = 3;

	gt::tree tr;
	auto err = cvt(joe, &tr);

	BOOST_REQUIRE(!err);

	BOOST_TEST(tr.value().empty());
	BOOST_TEST(tr.child_count() == 3);

	std::string name;
	std::uint8_t age = 0;

	cvt(tr.child(0), &name);
	cvt(tr.child(1), &age);

	BOOST_TEST(name == "joe");
	BOOST_TEST(age == 3);
}

BOOST_AUTO_TEST_CASE(EncodeWithErrorIsError)
{
	demo::employee joe;
	joe.name = "joe";
	joe.age = 3;
	joe.s.error = true;

	gt::tree tr;
	auto err = cvt(joe, &tr);

	BOOST_TEST(!!err);
}
