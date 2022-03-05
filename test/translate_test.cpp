#define BOOST_TEST_MODULE TranslaateTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(TranslateSelf)
{
	std::size_t in = 3, out = 0;
	auto err = gt::translate(in, &out);

	BOOST_TEST(!err);
	BOOST_TEST(out == 3);
}

BOOST_AUTO_TEST_CASE(TranslateToTreeAndBack)
{
	gt::tree tr;
	std::size_t in = 3, out = 0;
	auto err = gt::translate(in, &tr);
	auto err2 = gt::translate(tr, &out);

	BOOST_TEST(!err);
	BOOST_TEST(!err2);
	BOOST_TEST(out == 3);
}
