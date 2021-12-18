#define BOOST_TEST_MODULE EnumEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/enum.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"

namespace gt = gulachek::gtree;

enum class scoped_enum : std::uint8_t
{
	zero,
	one,
	two
};

enum unscoped_enum : std::uint8_t
{
	zero,
	one,
	two
};

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr;
	std::uint8_t n = 1;
	gt::encode(n, tr);

	scoped_enum scoped;
	unscoped_enum unscoped;

	BOOST_TEST(!gt::decode(tr, scoped));
	BOOST_TEST(!gt::decode(tr, unscoped));

	BOOST_CHECK(scoped == scoped_enum::one);
	BOOST_CHECK(unscoped == unscoped_enum::one);
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree tr;
	std::uint8_t scoped = 0, unscoped = 0;

	BOOST_TEST(!gt::encode(scoped_enum::two, tr));
	BOOST_TEST(!gt::decode(tr, scoped));

	BOOST_TEST(!gt::encode(unscoped_enum::two, tr));
	BOOST_TEST(!gt::decode(tr, unscoped));

	BOOST_TEST(scoped == 2);
	BOOST_TEST(unscoped == 2);
}
