#define BOOST_TEST_MODULE AsynctreederTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/asio/treeder.hpp"
#include "gulachek/gtree/asio/memory_read_stream.hpp"
#include <list>
#include <vector>
#include <type_traits>
#include <utility>

namespace asio = boost::asio;
namespace gt = gulachek::gtree;
namespace gta = gulachek::gtree::asio;

BOOST_AUTO_TEST_CASE(PerfectAlignmentCompletes)
{
	// A{B,C}
	std::vector<std::uint8_t> node_block_size = {7};
	std::vector<std::uint8_t> node_block = {
		3, // number of nodes
		1, 2,
		1, 0,
		1, 0,
	};

	std::vector<std::uint8_t> data_block = {
		'a', 'b', 'c'
	};

	asio::io_context io;
	gta::memory_read_stream stream{io};
	gta::treeder treeder{stream};

	stream.write_some(asio::buffer(node_block_size));
	stream.write_some(asio::buffer(node_block));
	stream.write_some(asio::buffer(data_block));

	auto ft = treeder.async_read(asio::use_future);

	io.run();

	auto t = ft.get();
}

BOOST_AUTO_TEST_CASE(OneBlockReadCompletes)
{
	// A{B,C}
	std::vector<std::uint8_t> bytes = {
		7, // size of node block
		3, // number of nodes
		1, 2,
		1, 0,
		1, 0,
		'a', 'b', 'c'
	};

	asio::io_context io;
	gta::memory_read_stream stream{io};
	gta::treeder treeder{stream};

	stream.write_some(asio::buffer(bytes));

	auto ft = treeder.async_read(asio::use_future);

	io.run();

	auto t = ft.get();
}
