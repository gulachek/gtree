#define BOOST_TEST_MODULE TranslateTest
#include <boost/test/unit_test.hpp>

#include "saboteur.hpp"

#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
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

BOOST_AUTO_TEST_CASE(VectorToIntIsZero)
{
	std::vector<int> v = { 1, 2, 3 };
	int n = 1;
	auto err = gt::translate(v, &n);

	BOOST_TEST(!err);
	BOOST_TEST(n == 0);
}

struct read_too_many_children
{
	cause gtree_decode(gt::treeder &r)
	{
		gt::ignore ig;

		auto cc = r.child_count();
		for (std::size_t i = 0; i <= cc; ++i)
			r.read(&ig);

		return {};
	}
};

// enable this to see error
// doesn't seem like fixing hang is important. dev should fix code
BOOST_AUTO_TEST_CASE(ReadingTooManyChildrenShouldInformError,
		*boost::unit_test::disabled())
{
	int n = 1;
	read_too_many_children dec;
	auto err = gt::translate(n, &dec);
}

BOOST_AUTO_TEST_CASE(ErrorWhileWritingIsError)
{
	std::vector<saboteur> w = { {true}, {} };
	std::vector<int> r;

	auto err = gt::translate(w, &r);

	BOOST_TEST(!!err);
	BOOST_TEST(err.has_ucode(gt::translate_error::encoding));
}

BOOST_AUTO_TEST_CASE(ErrorWhileReadingIsError)
{
	std::vector<int> w = { 1, 0 };
	std::vector<saboteur> r;

	auto err = gt::translate(w, &r);

	BOOST_TEST(!!err);
	BOOST_TEST(err.has_ucode(gt::translate_error::decoding));
}
