#define BOOST_TEST_MODULE gnatz_iterTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gnatz_iter.hpp"
#include <list>
#include <vector>
#include <list>
#include <type_traits>
#include <utility>

namespace gt = gulachek::gtree;

template <typename ... Bytes>
auto readNums(Bytes ... bytes)
{
	std::vector<std::uint8_t> vec = {static_cast<std::uint8_t>(bytes)...};
	gt::gnatz_iter iter{vec.data(), vec.size()};
	std::list<std::size_t> lst;

	std::size_t n;
	while (iter.next(&n))
		lst.push_back(n);

	return lst;
}

BOOST_AUTO_TEST_CASE(FormattedCorrectlyIteratesNums)
{
	auto nums = readNums(2, 0x80, 1, 3, 0x80, 0x80, 35);
	std::list<std::size_t> expect = {2, 1 << 7, 3, 35 << 14};

	BOOST_TEST(nums == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(NonTerminatedThrows)
{
	auto go = []{
		readNums(2, 3, 4, 0x80);
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(OutOfRangeThrows)
{
	auto go = []{
		std::vector<std::uint8_t> bytes;
		for (auto i = 0; i < sizeof(std::size_t); i++)
			bytes.push_back(0x80);
		bytes.push_back(1);

		gt::gnatz_iter iter{bytes.data(), bytes.size()};
		std::size_t n;
		while (iter.next(&n));
	};

	BOOST_CHECK_THROW(go(), std::exception);
}
