#define BOOST_TEST_MODULE GtreeWriterTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/gtree_writer.hpp"
#include "gulachek/gtree/encoding/string.hpp"

#include "copy_tree.hpp"
#include "string_tree.hpp"

#include <list>
#include <vector>
#include <type_traits>
#include <utility>
#include <sstream>

namespace gt = gulachek::gtree;

struct FakeBFS
{
	private:
		typedef void _Handler(
				const gt::block &value,
				std::size_t child_count
				);

	public:
		typedef std::function<_Handler> Visitor;

		std::function<void(
				const CopyTree &t, const Visitor &)> on_visit;
		template <typename Tree>
		void visit(const Tree &tree, const Visitor &visitor)
		{
			if (on_visit) on_visit(CopyTree{tree}, visitor);
		}
};

struct FakeNumberWriter
{
	std::function<std::size_t(std::size_t)> on_plan;
	std::size_t plan(std::size_t n)
	{
		if (on_plan) return on_plan(n);
		return 0;
	}
	
	std::function<void(std::size_t)> on_write;
	void write(std::size_t n)
	{
		if (on_write) on_write(n);
	}
};

struct FakeStream
{
	FakeStream& write(const char *s, std::size_t n){ return *this; }
	FakeStream& flush() { return *this; }
};

BOOST_AUTO_TEST_CASE(WritesTheSizeOfNodeBlockAsNumber)
{
	FakeBFS _bfs;
	FakeNumberWriter _nums;
	FakeStream _os;
	gt::gtree_writer _writer{_bfs, _nums, _os};

	std::string message;
	_bfs.on_visit = [&](
			const CopyTree &t,
			const FakeBFS::Visitor &f
			){

		gt::decode(t, message);

		// completely arbitrary
		f({nullptr, 23}, 81);
		f({nullptr, 986}, 10);
	};

	_nums.on_plan = [&](std::size_t n){
		return 2 * n;
	};

	std::vector<std::size_t> written;
	_nums.on_write = [&](std::size_t n){
		written.push_back(n);
	};

	_writer.write(StringTree{"hello"});

	// assume passing same message through means tree was passed to bfs
	BOOST_TEST(message == "hello");

	// assume passing 
	std::size_t total = 2 * (2 + 23 + 81 + 986 + 10); // these are all passed to node block
	BOOST_TEST(total == written[0]);
}

BOOST_AUTO_TEST_CASE(WritesTheNodeCountAsNumber)
{
	FakeBFS _bfs;
	FakeNumberWriter _nums;
	FakeStream _os;
	gt::gtree_writer _writer{_bfs, _nums, _os};

	_bfs.on_visit = [&](
			const CopyTree &t,
			const FakeBFS::Visitor &f
			){

		auto write = [&](const std::string &s){
		};
		// completely arbitrary ( 7 nodes here )
		f({nullptr, 23}, 81);
		f({nullptr, 986}, 10);
		f({nullptr, 986}, 10);
		f({nullptr, 986}, 10);
		f({nullptr, 986}, 10);
		f({nullptr, 986}, 10);
		f({nullptr, 986}, 10);
	};

	std::vector<std::size_t> written;
	_nums.on_write = [&](std::size_t n){
		written.push_back(n);
	};

	_writer.write(StringTree{"doesn't matter"});

	BOOST_TEST(7 == written[1]);
}

BOOST_AUTO_TEST_CASE(WritesEachSizeChildCountPairBFS)
{
	FakeBFS _bfs;
	FakeNumberWriter _nums;
	FakeStream _os;
	gt::gtree_writer _writer{_bfs, _nums, _os};

	_bfs.on_visit = [&](
			const CopyTree &t,
			const FakeBFS::Visitor &f
			){
		// completely arbitrary ( 7 nodes here )
		f({nullptr, 1}, 1);
		f({nullptr, 2}, 3);
		f({nullptr, 5}, 8);
		f({nullptr, 13}, 21);
		f({nullptr, 34}, 55);
		f({nullptr, 89}, 144);
		f({nullptr, 233}, 377);
	};

	std::list<std::size_t> written;
	_nums.on_write = [&](std::size_t n){
		written.push_back(n);
	};

	_writer.write(StringTree{"doesn't matter"});

	// node block size and node count
	written.pop_front(); written.pop_front();

	std::list<std::size_t> expect = {
		1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 377
	};

	BOOST_TEST(written == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(WritesEachValueBFS)
{
	FakeBFS _bfs;
	FakeNumberWriter _nums;
	std::ostringstream _os;
	gt::gtree_writer _writer{_bfs, _nums, _os};

	_bfs.on_visit = [&](
			const CopyTree &t,
			const FakeBFS::Visitor &f
			){

		auto v = [&](const std::string &s){
			f(gt::block{s.data(), s.size()}, 0);
		};

		v("un");
		v("to");
		v("free");
		v("thor");
		v("thithe");
	};

	_writer.write(StringTree{"doesn't matter"});

	BOOST_TEST(_os.str() == "untofreethorthithe");
}

struct spy_flush_ostream
{
	public:
		spy_flush_ostream() : _count{0} {}

		spy_flush_ostream& flush()
		{
			++_count;
			return *this;
		}

		std::size_t count() const
		{
			return _count;
		}

		spy_flush_ostream& write(const char* s, std::streamsize n)
		{
			return *this;
		}

	private:
		std::size_t _count;
};

BOOST_AUTO_TEST_CASE(FlushesOnce)
{
	FakeBFS _bfs;
	FakeNumberWriter _nums;
	spy_flush_ostream _os;
	gt::gtree_writer _writer{_bfs, _nums, _os};

	_writer.write(StringTree{"doesn't matter"});

	BOOST_TEST(_os.count() == 1);
}
