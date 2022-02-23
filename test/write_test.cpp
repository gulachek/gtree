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

BOOST_AUTO_TEST_CASE(EmptyTree)
{
	gt::tree tr;

	std::ostringstream os;
	auto err = gt::write(os, tr);

	BOOST_TEST(!err);
	BOOST_TEST(os.str() == "\x00\x00"s);
}
