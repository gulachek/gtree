#define BOOST_TEST_MODULE StringTest
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/read.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using gulachek::error;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeEmptyTreeIsEmptyStr)
{
	gt::tree tr;
	std::string s = "hello";
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &s);

	BOOST_TEST(!err);
	BOOST_TEST(s.empty());
}

BOOST_AUTO_TEST_CASE(DecodeHelloTree)
{
	vec val = {'h', 'e', 'l', 'l', 'o'};
	gt::tree tr{ val };

	std::string s;
	std::stringstream ss;

	gt::write(ss, tr);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &s);

	BOOST_TEST(!err);
	BOOST_TEST(s == "hello");
}

BOOST_AUTO_TEST_CASE(EncodeEmptyTree)
{
	vec val = {'a'};
	gt::tree tr{ val };
	std::string s;
	std::stringstream ss;

	gt::write(ss, s);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	BOOST_TEST(!err);
	BOOST_TEST(tr.value().empty());
	BOOST_TEST(tr.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(EncodeHelloTree)
{
	gt::tree tr;
	std::string s = "hello";
	std::stringstream ss;

	gt::write(ss, s);
	ss.seekg(0, std::ios::beg);
	auto err = gt::read(ss, &tr);

	auto val = tr.value();
	auto start = (const char*)val.data();
	std::string out{start, start + val.size()};

	BOOST_TEST(!err);
	BOOST_TEST(out == "hello");
	BOOST_TEST(tr.child_count() == 0);
}
