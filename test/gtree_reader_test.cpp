#define BOOST_TEST_MODULE gtree_readerTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gtree_reader.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <functional>
#include <sstream>
#include <queue>

namespace gt = gulachek::gtree;

class MemoryNatNumReader
{
	public:
		MemoryNatNumReader() : _nums{}, _err{false} {}

		void write(std::size_t n)
		{
			_nums.push(n);
		}

		void error() { _err = true; }

		bool read(std::size_t *n)
		{
			if (_err) throw std::runtime_error{"Error condition set"};

			if (_nums.empty()) return false;

			*n = _nums.front();
			_nums.pop();
			return true;
		}

	private:
		std::queue<std::size_t> _nums;
		bool _err;
};

gt::block str2blk(const std::string &s)
{
	return {s.data(), s.size()};
}

struct FakeTreeBuilder
{
	std::string _data_block;
	std::string _node_block;

	// Given a node block, where/how big is data block?
	std::function<gt::block(const gt::block &)> on_node_block;
	gt::block node_block(const gt::block &b)
	{
		// potential override
		if (on_node_block) return on_node_block(b);

		// interested in value most of the time
		_node_block = std::to_string(b);
		return str2blk(_data_block);
	}


	// Notify data block is ready and receive built tree
	std::function<gt::tree()> on_data_block;
	gt::tree data_block()
	{
		if (on_data_block) return on_data_block();
		return {};
	}
};

BOOST_AUTO_TEST_CASE(NodeBlockIsPassedToBuilder)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(4);
	stream << "node data";

	gt::tree t;

	BOOST_TEST(trees.read(&t));
	BOOST_TEST(builder._node_block == "node");
}

BOOST_AUTO_TEST_CASE(ZeroSizeNodeBlockResultsInEmptyRoot)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(0);

	gt::tree tree;

	BOOST_TEST(trees.read(&tree));
	BOOST_TEST(is_empty(tree));
}

BOOST_AUTO_TEST_CASE(DataBlockIsPassedToBuilder)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(5); // node block size "node "
	builder._data_block.resize(4); // data block size "data"

	stream << "node data";

	gt::tree tree;
	BOOST_TEST(trees.read(&tree));
	BOOST_TEST(builder._data_block == "data");
}

BOOST_AUTO_TEST_CASE(ErrReadingSizeIsErrInCompletion)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.error();
	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(EofReadingNodeBlockIsBadMessage)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(4);

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(ReadLessThanNodeBlockIsNonEofError)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(10);
	stream << "hello";

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(EofReadingDataBlockIsBadMessage)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(1);
	stream << '1';

	builder._data_block.resize(1); // data block size

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(ReadLessThanDataBlockIsNonEofError)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(1); // node block size
	builder._data_block.resize(32); // data block size
	stream << "hello"; // node + data blocks

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(ParsingNodeBlockThrowsResultsInNonEofError)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(1); // node block size
	builder.on_node_block = [&](const gt::block &){
		throw std::runtime_error{"take that"};
		return gt::block{};
	};
	stream << "hello"; // node + data blocks

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(ParsingDataBlockThrowsResultsInNonEofError)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	nums.write(1); // node block size
	builder._data_block.resize(1);
	builder.on_data_block = []{
		throw std::runtime_error{"gotta get get boom boom BOOM"};
		return gt::tree{};
	};
	stream << "hello"; // node + data blocks

	gt::tree tree;

	BOOST_CHECK_THROW(trees.read(&tree), std::exception);
}

BOOST_AUTO_TEST_CASE(EOFAtNodeBlockSizeIsEOF)
{
	std::stringstream stream;
	MemoryNatNumReader nums;
	FakeTreeBuilder builder;
	gt::gtree_reader trees{nums, stream, builder};

	gt::tree tree;

	BOOST_TEST(!trees.read(&tree));
}
