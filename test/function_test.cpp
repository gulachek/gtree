#define BOOST_TEST_MODULE FunctionTest
#include <boost/test/unit_test.hpp>

#include "saboteur.hpp"

#include "gulachek/gtree/translate.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/function.hpp"
#include <vector>
#include <sstream>
#include <cstdint>

namespace tt = boost::test_tools;

using cause = gulachek::cause;
namespace gt = gulachek::gtree;

using vec = std::vector<std::uint8_t>;

BOOST_AUTO_TEST_CASE(DecodeVector)
{
	vec v = { 1, 2, 3 };
	vec out;

	auto f = [&](gt::treeder &r){
		gt::decoding<vec> dec{&out};
		return dec.decode(r);
	};
	auto err = gt::translate(v, &f);

	BOOST_TEST(!err);
	BOOST_TEST(out == v, tt::per_element());
}

BOOST_AUTO_TEST_CASE(ReadSendErrorIsErr)
{
	int n = 0;

	auto f = [&](gt::treeder &r){
		return cause{"ad hoc"};
	};
	auto err = gt::translate(n, &f);

	BOOST_TEST(!!err);
}

BOOST_AUTO_TEST_CASE(EncodeVector)
{
	vec v = { 1, 2, 3 };
	vec out;

	auto f = [&](gt::tree_writer &w){
		gt::encoding<vec> enc{v};
		return enc.encode(w);
	};

	auto err = gt::translate(f, &out);

	BOOST_TEST(!err);
	BOOST_TEST(out == v, tt::per_element());
}

BOOST_AUTO_TEST_CASE(WriteSendErrorIsErr)
{
	int n;

	auto f = [&](gt::tree_writer &w){
		return cause{"ad hoc"};
	};
	auto err = gt::translate(f, &n);

	BOOST_TEST(!!err);
}
