#define BOOST_TEST_MODULE WriteTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include <sstream>
#include <vector>
#include <cstdint>
#include <string>

namespace gt = gulachek::gtree;
using namespace std::string_literals;

using cause = gulachek::cause;

BOOST_AUTO_TEST_CASE(EmptyTree)
{
	gt::tree tr;

	std::ostringstream os;
	auto err = gt::write(os, tr);

	BOOST_TEST(!err);
	BOOST_TEST(os.str() == "\x00\x00"s);
}

struct write_cc_before_value
{
	cause gtree_encode(gt::tree_writer &w) const
	{
		w.child_count(0);
		int x = 3;
		w.value(&x, sizeof(x));
		return {};
	}
};

BOOST_AUTO_TEST_CASE(WritingChildCountBeforeValueIsLogicError)
{
	auto go = []{
		write_cc_before_value x;

		std::ostringstream os;
		gt::write(os, x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

struct write_value_twice
{
	cause gtree_encode(gt::tree_writer &w) const
	{
		int x = 3;
		w.value(&x, sizeof(x));
		w.value(&x, sizeof(x));
		w.child_count(0);
		return {};
	}
};

BOOST_AUTO_TEST_CASE(WritingValueTwiceIsLogicError)
{
	auto go = []{
		write_value_twice x;

		std::ostringstream os;
		gt::write(os, x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

struct write_child_before_count
{
	cause gtree_encode(gt::tree_writer &w) const
	{
		int x = 3;
		w.value(&x, sizeof(x));

		gt::tree child;
		w.write(child);

		w.child_count(1);
		return {};
	}
};

BOOST_AUTO_TEST_CASE(WritingChildBeforeCountIsLogicError)
{
	auto go = []{
		write_child_before_count x;

		std::ostringstream os;
		gt::write(os, x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

struct write_too_many_children
{
	cause gtree_encode(gt::tree_writer &w) const
	{
		int x = 3;
		w.value(&x, sizeof(x));

		w.child_count(1);

		gt::tree child;
		w.write(child);
		w.write(child);

		return {};
	}
};

BOOST_AUTO_TEST_CASE(WritingTooManyChildrenIsLogicError)
{
	auto go = []{
		write_too_many_children x;

		std::ostringstream os;
		gt::write(os, x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

struct write_too_few_children
{
	cause gtree_encode(gt::tree_writer &w) const
	{
		int x = 3;
		w.value(&x, sizeof(x));

		w.child_count(1);

		return {};
	}
};

BOOST_AUTO_TEST_CASE(WritingTooFewChildrenIsLogicError)
{
	auto go = []{
		write_too_few_children x;

		std::ostringstream os;
		gt::write(os, x);
	};

	BOOST_CHECK_THROW(go(), std::logic_error);
}

template <typename T>
std::span<const std::uint8_t> make_span(const T &v)
{
	auto start = (const std::uint8_t *)&v;
	return {start, start + sizeof(T)};
}

BOOST_AUTO_TEST_CASE(AbcTree)
{
	char a = 'a', b = 'b', c = 'c';

	gt::tree tr;
	tr.value(make_span(a));
	tr.child_count(2);
	tr.child(0).value(make_span(b));
	tr.child(1).value(make_span(c));

	std::ostringstream os;
	gt::write(os, tr);

	BOOST_TEST(os.str() == "\x01\x61\x02\x01\x62\x00\x01\x63\x00"s);
}
