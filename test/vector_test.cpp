#define BOOST_TEST_MODULE VectorTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "saboteur.hpp"

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeEmptyTreeIsEmptyVec)
{
	gt::tree tr;
	vec v = { 1, 2, 3 };
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &v);

	BOOST_TEST(!err);
	BOOST_TEST(v.empty());
}

BOOST_AUTO_TEST_CASE(DecodeWithChildren)
{
	vec a = { 'a' };
	vec b = { 'b' };
	vec c = { 'c' };

	gt::tree tr{ vec{}, std::vector<gt::tree>{
		{ a },
		{ b },
		{ c }
	}};

	vec v;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &v);

	BOOST_TEST(!err);

	vec expect = { 'a', 'b', 'c' };
	BOOST_TEST(v == expect, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeErrorInElemIsError)
{
	vec fail = { 'f' };
	gt::tree tr{ vec{}, std::vector<gt::tree>{
		{},
		{ fail },
		{}
	}};

	std::vector<saboteur> v;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &v);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeEmptyIsEmpty)
{
	vec v;
	vec placeholder = { 'a', 'b', 'c' };
	gt::tree tr{ placeholder };

	std::stringstream ss;
	auto err = gt::write(ss, v);

	BOOST_REQUIRE(!err);

	ss.seekg(0, std::ios::beg);
	gt::read(ss, &tr);

	BOOST_TEST(tr.empty());
}

BOOST_AUTO_TEST_CASE(EncodeWithElems)
{
	vec v = { 'a', 'b', 'c' };
	gt::tree tr;

	std::stringstream ss;
	auto err = gt::write(ss, v);

	BOOST_REQUIRE(!err);

	ss.seekg(0, std::ios::beg);
	gt::read(ss, &tr);

	BOOST_TEST(tr.value().empty());
	BOOST_REQUIRE(tr.child_count() == 3);

	auto a = tr.child(0);
	auto b = tr.child(1);
	auto c = tr.child(2);

	BOOST_TEST(a.child_count() == 0);
	BOOST_TEST(b.child_count() == 0);
	BOOST_TEST(c.child_count() == 0);

	BOOST_TEST(a.value()[0] == 'a');
	BOOST_TEST(b.value()[0] == 'b');
	BOOST_TEST(c.value()[0] == 'c');
}

BOOST_AUTO_TEST_CASE(EncodeErrorInElemIsError)
{
	std::vector<saboteur> v;
	v.resize(3);
	v[1].error = true;

	gt::tree tr;

	std::stringstream ss;
	auto err = gt::write(ss, v);

	BOOST_TEST(!!err);
}
