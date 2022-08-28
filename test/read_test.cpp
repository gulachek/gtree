#define BOOST_TEST_MODULE ReadTest
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/tree.hpp"
#include <strstream>
#include <sstream>
#include <vector>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(EmptyTree)
{
	std::vector<std::uint8_t> buf = {0, 0};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!err);

	auto val = tr.value();
	BOOST_TEST(val.empty());

	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EmptySeqIsEof)
{
	std::stringstream ss;

	gt::tree tr;
	auto err = gt::read(ss, &tr);

	BOOST_TEST(err.is_eof());
}

BOOST_AUTO_TEST_CASE(AbcValue)
{
	std::vector<std::uint8_t> buf = {3, 'a', 'b', 'c', 0};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!err);

	auto val = tr.value();
	BOOST_TEST(val.size() == 3);
	BOOST_TEST(val[0] == 'a');
	BOOST_TEST(val[1] == 'b');
	BOOST_TEST(val[2] == 'c');

	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(IncompleteValueSizeIsError)
{
	std::vector<std::uint8_t> buf = {128};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!!err);
	BOOST_CHECK(err.has_ucode(gt::read_error::incomplete_value_size));
}

BOOST_AUTO_TEST_CASE(IncompleteValueIsError)
{
	std::vector<std::uint8_t> buf = {3, 'a', 'b'};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!!err);
	BOOST_CHECK(err.has_ucode(gt::read_error::incomplete_value));
}

BOOST_AUTO_TEST_CASE(IncompleteChildCountError)
{
	std::vector<std::uint8_t> buf = {3, 'a', 'b', 'c', 128};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!!err);
	BOOST_CHECK(err.has_ucode(gt::read_error::incomplete_child_count));
}

BOOST_AUTO_TEST_CASE(MissingChildIsError)
{
	std::vector<std::uint8_t> buf = {3, 'a', 'b', 'c', 1};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree tr;
	auto err = gt::read(is, &tr);

	BOOST_TEST(!!err);
	BOOST_CHECK(err.has_ucode(gt::read_error::bad_children));
}

BOOST_AUTO_TEST_CASE(AbcChildren)
{
	std::vector<std::uint8_t> buf = {
		1, 'a', 2,
		1, 'b', 0,
		1, 'c', 0
	};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	gt::tree a;
	auto err = gt::read(is, &a);

	BOOST_TEST(!err);

	auto aval = a.value();
	BOOST_TEST(aval.size() == 1);
	BOOST_TEST(aval[0] == 'a');

	BOOST_TEST(a.child_count() == 2);

	auto b = a.child(0);
	auto bval = b.value();
	BOOST_TEST(bval.size() == 1);
	BOOST_TEST(bval[0] == 'b');

	auto c = a.child(1);
	auto cval = c.value();
	BOOST_TEST(cval.size() == 1);
	BOOST_TEST(cval[0] == 'c');
}

struct read_first_child
{
	cause gtree_decode(gt::treeder &r)
	{
		gt::ignore child;
		return r.read(&child);
	}
};

BOOST_AUTO_TEST_CASE(ReadTooManyChildrenThrowsLogicError)
{
	auto go = []{

		std::vector<std::uint8_t> buf = {
			0, 0,
		};
		std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

		read_first_child x;
		gt::read(is, &x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

BOOST_AUTO_TEST_CASE(ReadTooFewChildrenIsAccountedFor)
{
	std::vector<std::uint8_t> buf = {
		0, 2,
		0, 0,
		0, 0,
	};
	std::istrstream is{(char*)buf.data(), (std::streamsize)buf.size()};

	read_first_child x;
	auto err = gt::read(is, &x);
	BOOST_CHECK(!err);

	gt::tree tr;
	err = gt::read(is, &tr);

	BOOST_CHECK(err.is_eof());
}
