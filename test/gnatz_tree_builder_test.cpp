#define BOOST_TEST_MODULE gnatz_tree_builderTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gnatz_tree_builder.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <initializer_list>

namespace gt = gulachek::gtree;

class VecIter
{
	public:
		VecIter(std::initializer_list<std::size_t> il) :
			_index{0},
			_nums{il}
		{}

		bool next(std::size_t *n)
		{
			if (_index >= _nums.size())
				return false;

			*n = _nums[_index++];
			return true;
		}

	private:
		std::size_t _index;
		std::vector<std::size_t> _nums;
};

BOOST_AUTO_TEST_SUITE(NodeBlock)

BOOST_AUTO_TEST_CASE(FirstNumIsNodeCount)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	tb.node_block(VecIter{3, 0, 2, 0, 0, 0, 0});

	BOOST_TEST(tb.node_count() == 3);
}

BOOST_AUTO_TEST_CASE(FirstNumForEachNodeIsSize)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	tb.node_block(VecIter{3, 1, 2, 2, 0, 3, 0});

	BOOST_TEST(tb.node_size(0) == 1);
	BOOST_TEST(tb.node_size(1) == 2);
	BOOST_TEST(tb.node_size(2) == 3);
}

BOOST_AUTO_TEST_CASE(NodeAddressIsOffsetIntoDataBlock)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	tb.node_block(VecIter{3, 1, 2, 2, 0, 3, 0});

	auto data = (const std::uint8_t *) tb.data_address();
	BOOST_TEST(tb.node_address(0) == &data[0]);
	BOOST_TEST(tb.node_address(1) == &data[1]);
	BOOST_TEST(tb.node_address(2) == &data[3]);
}

BOOST_AUTO_TEST_CASE(SecondNumIsChildCount)
{
	gt::gnatz_tree_builder tb;

	// A{B{D, E{G}}, C{F{H}}}
	tb.node_block(VecIter{8,
			0, 2, // A
			0, 2, // B
			0, 1, // C
			0, 0, // D
			0, 1, // E
			0, 1, // F
			0, 0, // G
			0, 0, // H
			});

	BOOST_TEST(tb.child_count(0) == 2);
	BOOST_TEST(tb.child_count(1) == 2);
	BOOST_TEST(tb.child_count(2) == 1);
	BOOST_TEST(tb.child_count(3) == 0);
	BOOST_TEST(tb.child_count(4) == 1);
	BOOST_TEST(tb.child_count(5) == 1);
	BOOST_TEST(tb.child_count(6) == 0);
	BOOST_TEST(tb.child_count(7) == 0);
}

BOOST_AUTO_TEST_CASE(FirstChildIsCumulativeSumOfSecondNumStartingAt1)
{
	gt::gnatz_tree_builder tb;

	// A{B{D, E{G}}, C{F{H}}}
	tb.node_block(VecIter{8,
			0, 2, // A
			0, 2, // B
			0, 1, // C
			0, 0, // D
			0, 1, // E
			0, 1, // F
			0, 0, // G
			0, 0, // H
			});

	BOOST_TEST(tb.first_child(0) == 1); // A -> B
	BOOST_TEST(tb.first_child(1) == 3); // B -> D
	BOOST_TEST(tb.first_child(2) == 5); // C -> F
	// D unspecified
	BOOST_TEST(tb.first_child(4) == 6); // E -> G
	BOOST_TEST(tb.first_child(5) == 7); // F -> H
	// G, H unspecified
}

BOOST_AUTO_TEST_CASE(ReturnedSizeIsSumOfSizes)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	auto data_block = tb.node_block(VecIter{3,
			1, 2, // A
			2, 0, // B
			3, 0, // C
			});

	BOOST_TEST(data_block.size() == 6);
}

BOOST_AUTO_TEST_CASE(ReturnedSizeIsSameAsDataBlockSize)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	auto data_block = tb.node_block(VecIter{3,
			1, 2, // A
			2, 0, // B
			3, 0, // C
			});

	BOOST_TEST(data_block.size() == tb.data_size());
}

BOOST_AUTO_TEST_CASE(ReturnedAddressIsDataBlockAddress)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	auto data_block = tb.node_block(VecIter{3,
			1, 2, // A
			2, 0, // B
			3, 0, // C
			});

	BOOST_TEST(data_block.data() == tb.data_address());
}

BOOST_AUTO_TEST_CASE(ZeroNodesReturnsZeroSizeDataBlock)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	auto data_block = tb.node_block(VecIter{0});

	BOOST_TEST(data_block.empty());
}

BOOST_AUTO_TEST_CASE(NoNodeBlockReturnsZeroSizeDataBlock)
{
	gt::gnatz_tree_builder tb;

	// A{B, C}
	auto data_block = tb.node_block(VecIter{});

	BOOST_TEST(data_block.empty());
}

BOOST_AUTO_TEST_CASE(NodeCountTooLowThrows)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{1, // <-- should be 2
				0, 1, // A
				0, 0, // B
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(NodeCountTooHighThrows)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{3, // <-- should be 2
				0, 1, // A
				0, 0, // B
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(ChildCountTooLowThrows)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{2,
				0, 0, // A <-- child count should be 1
				0, 0, // B
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(ChildCountTooHighThrows)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{2,
				0, 2, // A <-- child count should be 1
				0, 0, // B
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(MissingChildCountThrows)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{2,
				0, 1, // A
				0,    // B <-- no child count
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(MissingChildCountThrowsIfNodeCountIsLow)
{
	gt::gnatz_tree_builder tb;

	// A{B}
	auto go = [&]{
		tb.node_block(VecIter{1, // <-- should be 2
				0, 1, // A
				0,    // B <-- no child count
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_CASE(ExtraNumberAtEndThrows)
{
	gt::gnatz_tree_builder tb;

	// A{}
	auto go = [&]{
		tb.node_block(VecIter{1,
				0, 0, // A
				0,    // <-- extra
				});
	};

	BOOST_CHECK_THROW(go(), std::exception);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DataBlock,
		* boost::unit_test::depends_on("NodeBlock") // seems easy way to set up tests
		)

BOOST_AUTO_TEST_CASE(ZeroNodesEmitsEmptyTree)
{
	gt::gnatz_tree_builder tb;
	tb.node_block(VecIter{0});

	auto tr = tb.data_block();

	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_SUITE(NonZeroNodesEmitsTreeAtRootNode)

BOOST_AUTO_TEST_CASE(SameChildCount)
{
	gt::gnatz_tree_builder tb;
	tb.node_block(VecIter{3, 0, 2, 0, 0, 0, 0}); // A{B,C}

	auto tr = tb.data_block();

	BOOST_TEST(tr.child_count() == 2);
}

BOOST_AUTO_TEST_CASE(SameValue)
{
	gt::gnatz_tree_builder tb;
	auto data = tb.node_block(VecIter{3, 3, 2, 0, 0, 0, 0}); // A{B,C}

	auto v = (char *)data.data();
	v[0] = 'a';
	v[1] = 'b';
	v[2] = 'c';

	auto tr = tb.data_block();
	std::string abc; gt::decode(tr, abc);
	BOOST_TEST(abc == "abc");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_CASE(ClearsTree)
{
	gt::gnatz_tree_builder tb;
	tb.node_block(VecIter{3, 3, 2, 0, 0, 0, 0}); // A{B,C}

	tb.data_block(); // clear
	auto tr = tb.data_block();

	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_SUITE_END()
