#define BOOST_TEST_MODULE AsyncGtreeReaderTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/asio/gtree_reader.hpp"
#include "gulachek/gtree/asio/memory_read_stream.hpp"
#include "gulachek/gtree/asio/memory_message_reader.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <functional>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

using NatNumReader = gta::memory_message_reader<std::size_t>;

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

template <typename T = std::uint8_t>
struct BadAllocator
{
	typedef T value_type;

	BadAllocator() = default;
	template <typename U> constexpr BadAllocator(const BadAllocator<U> &)
		noexcept {}

	[[nodiscard]] T* allocate(std::size_t n) {
		throw std::bad_alloc();
	}

	void deallocate(T* p, std::size_t) noexcept {}
};

template <typename T, typename U>
bool operator == (const BadAllocator<T> &, const BadAllocator<U> &)
{ return true; }

template <typename T, typename U>
bool operator != (const BadAllocator<T> &, const BadAllocator<U> &)
{ return false; }

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(ExecutorTypeSameAsNatNumExecutorType)
{
	bool same_executor = std::is_same<
		NatNumReader::executor_type,
		gta::gtree_reader<NatNumReader, gta::memory_read_stream<>, FakeTreeBuilder>::executor_type
			>::value;

	BOOST_TEST(same_executor);
}

BOOST_AUTO_TEST_SUITE_END() // TypeTraits

BOOST_AUTO_TEST_SUITE(GetExecutor)

BOOST_AUTO_TEST_CASE(SameAsNatNumExecutor)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	BOOST_CHECK(nums.get_executor() == trees.get_executor());
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(ReadingTrees)

BOOST_AUTO_TEST_CASE(NodeBlockIsPassedToBuilder)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	auto tree = trees.async_read(asio::use_future);

	nums.write(4);
	stream.write_some(asio::buffer("node data"));

	io.run();
	tree.get();

	BOOST_TEST(builder._node_block == "node");
}

BOOST_AUTO_TEST_CASE(ZeroSizeNodeBlockResultsInEmptyTree)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	auto tree = trees.async_read(asio::use_future);

	nums.write(0);
	stream.error(asio::error::eof);

	io.run();

	BOOST_TEST(is_empty(tree.get()));
}

BOOST_AUTO_TEST_CASE(DataBlockIsPassedToBuilder)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	nums.write(5); // node block size "node "
	builder._data_block.resize(4); // data block size "data"

	auto tree = trees.async_read(asio::use_future);

	stream.write_some(asio::buffer("node data"));

	io.run();

	tree.get();

	BOOST_TEST(builder._data_block == "data");
}

BOOST_AUTO_TEST_CASE(Sequential)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};
	std::uint8_t a, b;
	gt::tree_base base{{{{&a, 0}, 0, 0}, {{&b, 0}, 0, 0}}, {}};
	auto pbase = std::make_shared<gt::tree_base>(std::move(base));

	gt::tree t1{pbase, 0};
	gt::tree t2{pbase, 1};

	std::string total_block = "node data second time";
	nums.write(std::string{"node "}.size());
	nums.write(std::string{"second "}.size());
	builder.on_node_block = [&](const gt::block &b)
	{
		auto str = std::to_string(b);

		if (str == "node ")
			builder._data_block.resize(5); // "data "
		else if (str == "second ")
			builder._data_block.resize(4); // "time"
		else
			throw std::runtime_error{"darn messed up"};

		return str2blk(builder._data_block);
	};

	builder.on_data_block = [&]()
	{
		auto &str = builder._data_block;

		if (str == "data ")
			return t1;
		else if (str == "time")
			return t2;
		else
			throw std::runtime_error{"close but no cigar"};
	};

	auto first = trees.async_read(asio::use_future);

	stream.write_some(asio::buffer(total_block));

	io.run();
	BOOST_TEST((first.get()).value().data() == &a);

	io.restart();

	auto second = trees.async_read(asio::use_future);

	io.run();
	BOOST_CHECK((second.get()).value().data() == &b);
}

boost::system::error_code codes[] = {
	asio::error::eof,
	gta::error::bad_message
};

BOOST_DATA_TEST_CASE(
		ErrReadingSizeIsErrInCompletion,
		bd::make(codes),
		err
		)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	nums.error(err);

	io.run();

	BOOST_TEST(ec == err);
}

BOOST_AUTO_TEST_CASE(EofReadingNodeBlockIsBadMessage)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	nums.write(4);
	stream.error(asio::error::eof);

	io.run();

	BOOST_TEST(ec == gta::error::bad_message);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(NonEofErrorReadingNodeBlockIsPassedToCompletion)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	nums.write(4);
	stream.error(asio::error::access_denied);

	io.run();

	BOOST_TEST(ec == asio::error::access_denied);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(ReadLessThanNodeBlockIsNonEofError)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	nums.write(10);

	stream.write_some(asio::buffer("hello"));
	stream.error(asio::error::eof);

	io.run();

	BOOST_TEST(!!ec);
	BOOST_TEST(ec != asio::error::eof);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(EofReadingDataBlockIsBadMessage)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	bool node_block_called = false;
	nums.write(1); // node block size
	builder._data_block.resize(1); // data block size

	int x;
	stream.write_some(asio::buffer(&x, 1));
	stream.error(asio::error::eof);

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(ec == gta::error::bad_message);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(NonEofErrorReadingDataBlockIsPassedToCompletion)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	nums.write(1); // node block size
	builder._data_block.resize(1);

	int x;
	stream.write_some(asio::buffer(&x, 1));
	stream.error(asio::error::fault);

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(ec == asio::error::fault);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(ReadLessThanDataBlockIsNonEofError)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	nums.write(1); // node block size
	builder._data_block.resize(32); // data block size
	stream.write_some(asio::buffer("hello")); // node + data blocks
	stream.error(asio::error::eof); // mark end

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(!!ec);
	BOOST_TEST(ec != asio::error::eof);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(ParsingNodeBlockThrowsResultsInNonEofError)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	nums.write(1); // node block size
	builder.on_node_block = [&](const gt::block &){
		throw std::runtime_error{"take that"};
		return gt::block{};
	};
	stream.write_some(asio::buffer("hello")); // node + data blocks

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(!!ec);
	BOOST_TEST(ec != asio::error::eof);
	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(ParsingDataBlockThrowsResultsInNonEofError)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	nums.write(1); // node block size
	builder._data_block.resize(1);
	builder.on_data_block = []{
		throw std::runtime_error{"gotta get get boom boom BOOM"};
		return gt::tree{};
	};
	stream.write_some(asio::buffer("hello")); // node + data blocks

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(!!ec);
	BOOST_TEST(ec != asio::error::eof);
	BOOST_TEST(is_empty(tr));
}

/*
BOOST_AUTO_TEST_CASE(FailureToAllocateNodeBlockResultsInNotEnoughMemory)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	BadAllocator alloc;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, alloc, sb};

	boost::system::error_code ec;
	gt::tree tr;

	nums.write(1); // node block size

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();

	BOOST_TEST(ec == gta::error::not_enough_memory);
}
*/

BOOST_AUTO_TEST_SUITE(ExternalStreamBuf)

BOOST_AUTO_TEST_CASE(DataInBufUsedAsNodeBlock)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	std::string pre = "he";
	asio::buffer_copy(sb.prepare(pre.size()), asio::buffer(pre));
	sb.commit(pre.size());

	nums.write(4); // node block size
	stream.write_some(asio::buffer("hello")); // node + data blocks

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();
	
	BOOST_TEST(builder._node_block == "hehe");
}

BOOST_AUTO_TEST_CASE(DataBlockFromPreExistingBuffer)
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	std::string pre = "hello world";
	asio::buffer_copy(sb.prepare(pre.size()), asio::buffer(pre));
	sb.commit(pre.size());

	nums.write(6); // "hello "
	stream.write_some(asio::buffer("sssssssssssssssssssss"));
	builder._data_block.resize(6);

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();
	
	BOOST_TEST(builder._data_block == "worlds");
}

BOOST_AUTO_TEST_CASE(EntireOpInBuf, * boost::unit_test::label("buggy"))
{
	asio::io_context io;
	gta::memory_read_stream stream{io};
	NatNumReader nums{io};
	FakeTreeBuilder builder;
	asio::streambuf sb;
	gta::gtree_reader trees{nums, stream, builder, sb};

	boost::system::error_code ec;
	gt::tree tr;

	std::vector<std::uint8_t> pre = {3, 1, 2, 1, 0, 1, 0, 'a', 'b', 'c'};
	asio::buffer_copy(sb.prepare(pre.size()), asio::buffer(pre));
	sb.commit(pre.size());

	nums.write(7); // "hello "
	builder._data_block.resize(3);

	trees.async_read([&]
			(const boost::system::error_code &_ec, gt::tree &&_tr)
			{ ec = _ec; tr = _tr; }
			);

	io.run();
	
	BOOST_TEST(builder._data_block == "abc");
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END() // ReadingTrees
