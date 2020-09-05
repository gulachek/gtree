#define BOOST_TEST_MODULE BFSTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/bfs.hpp"

#include "copy_tree.hpp"
#include "string_tree.hpp"

#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <sstream>

namespace gt = gulachek::gtree;

class TestTree
{
	public:
		TestTree(std::size_t n) : _n{n} {}

		const gt::block value() const
		{ return {nullptr, _n}; }

		std::size_t child_count() const
		{ return _n; }

		TestTree child(std::size_t i) const
		{ return {_n - i - 1}; }

	private:
		std::size_t _n;
};

template <typename Tree>
std::list<std::size_t> visit(const Tree &t)
{
	std::list<std::size_t> ns;

	gt::bfs bfs;
	bfs.visit(t,
		[&](const gt::block &b, std::size_t n){
		ns.push_back(n);
	});

	return ns;
}

BOOST_AUTO_TEST_CASE(SingleNodeTree)
{
	TestTree tree{0};
	std::list<std::size_t> expect = {0};

	BOOST_TEST(visit(tree) == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(TwoNode)
{
	TestTree tree{1};
	std::list<std::size_t> expect = {1, 0};

	BOOST_TEST(visit(tree) == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(FourNode)
{
	TestTree tree{2};
	std::list<std::size_t> expect = {2, 1, 0, 0};

	BOOST_TEST(visit(tree) == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(Three)
{
	TestTree tree{3};
	std::list<std::size_t> expect =
	{3,
		2, 1, 0,
		1, 0, 0,
		0};

	BOOST_TEST(visit(tree) == expect, tt::per_element());
}
