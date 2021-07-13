#define BOOST_TEST_MODULE EncodingErrorTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/error.hpp"

#include <sstream>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(DefaultIsFalsy)
{
	gt::error dflt;
	BOOST_TEST(!dflt);
}

BOOST_AUTO_TEST_CASE(ConstructWithStringIsTruthy)
{
	gt::error dflt{"some error"};
	BOOST_TEST(dflt);
}

BOOST_AUTO_TEST_CASE(ConstructWithConstCharStar)
{
	gt::error dflt = "some error";
	BOOST_TEST(dflt);
}

BOOST_AUTO_TEST_CASE(FormatWritesMessage)
{
	gt::error err = "hello";
	std::ostringstream os;
	err.format(os);
	BOOST_TEST(os.str() == "hello");
}

BOOST_AUTO_TEST_CASE(FormatOperatorWritesMessage)
{
	gt::error err = "hello";
	std::ostringstream os;
	os << err;
	BOOST_TEST(os.str() == "hello");
}

BOOST_AUTO_TEST_CASE(FormattedInputForError)
{
	gt::error err;
	err << "Hello " << 42;
	std::ostringstream os;
	err.format(os);
	BOOST_TEST(os.str() == "Hello 42");
}
