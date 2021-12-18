#define BOOST_TEST_MODULE MapEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/map.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/variant.hpp"
#include "gulachek/gtree/encoding/optional.hpp"

#include <vector>
#include <optional>
#include <cstdint>

namespace gt = gulachek::gtree;

using strsize = std::map<std::string, std::size_t>;

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	gt::mutable_tree tr;

	strsize map = {{"hello", 42}};
	gt::decode(tr, map);

	BOOST_TEST(map.empty());
}

BOOST_AUTO_TEST_CASE(Decode)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{{
			gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }},
			gt::mutable_tree{{ 42 }}
		}},
			gt::mutable_tree{{
			gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }},
			gt::mutable_tree{{ 24 }}
		}},
	}};

	strsize map;
	gt::decode(tr, map);

	BOOST_TEST(map.size() == 2);
	BOOST_TEST(map["hello"] == 42);
	BOOST_TEST(map["world"] == 24);
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
	gt::mutable_tree tr{{ gt::mutable_tree{} }};

	strsize map;
	gt::encode(map, tr);

	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(Encode)
{
	gt::mutable_tree expect{{
		gt::mutable_tree{{
			gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }},
			gt::mutable_tree{{ 42 }}
		}},
			gt::mutable_tree{{
			gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }},
			gt::mutable_tree{{ 24 }}
		}},
	}};

	gt::mutable_tree result;

	strsize map = {
		{ "hello", 42 },
		{ "world", 24 }
	};

	gt::encode(map, result);

	BOOST_CHECK(result == expect);
}
