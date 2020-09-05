#define BOOST_TEST_MODULE gnatz_writerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gnatz_writer.hpp"

#include "copy_tree.hpp"
#include "string_tree.hpp"

#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <sstream>

namespace gt = gulachek::gtree;

typedef std::list<std::uint8_t> NumList;

class ListStream
{
	public:
		NumList list() const
		{
			return _list;
		}

		void write(const char* s, std::size_t n)
		{
			for (auto i = 0; i < n; i++)
				_list.push_back((std::uint8_t)s[i]);
		}

	private:
		NumList _list;
};

NumList write(std::size_t n)
{
	ListStream stream;
	gt::gnatz_writer gnz{stream};
	gnz.write(n);

	auto out = stream.list();

	// These must always be the same
	BOOST_TEST(out.size() == gnz.plan(n));

	return out;
}

BOOST_AUTO_TEST_CASE(Zero)
{
	auto zero = write(0);
	NumList expect = {0};

	BOOST_TEST(zero == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(ThirtyTwo)
{
	auto out = write(32);
	NumList expect = {32};

	BOOST_TEST(out == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(TwoFiftySix)
{
	auto out = write(256);
	NumList expect = {0x80, 0x02};

	BOOST_TEST(out == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(ShiftFifty)
{
	auto out = write(1LU << 50LU);
	NumList expect =
		{0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 2};

	BOOST_TEST(out == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(MaxInt)
{
	// Implicit assumption in this test
	BOOST_REQUIRE(sizeof(std::size_t) == 8);

	auto out = write(~0LU);
	NumList expect =
		{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
			0xff, 0xff, 1};

	BOOST_TEST(out == expect, tt::per_element());
}
