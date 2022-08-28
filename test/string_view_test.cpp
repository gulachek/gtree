#define BOOST_TEST_MODULE StringViewTest
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/encoding/string_view.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(NotDecodable)
{
	BOOST_TEST(!gt::decodable<std::string_view>);
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
	std::string_view s;
	std::string out;

	auto err = gt::translate(s, &out);

	BOOST_TEST(!err);
	BOOST_TEST(out.empty());
}

BOOST_AUTO_TEST_CASE(EncodeHello)
{
	std::string_view s = "hello";
	std::string out;

	auto err = gt::translate(s, &out);

	BOOST_TEST(!err);
	BOOST_TEST(out == "hello");
}
