#define BOOST_TEST_MODULE MapEncodingTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <vector>
#include <optional>
#include <cstdint>

namespace gt = gulachek::gtree;

using strsize = std::map<std::string, std::size_t>;

BOOST_AUTO_TEST_CASE(NoValue)
{
	BOOST_TEST(!gt::uses_value<strsize>::value);
}

BOOST_AUTO_TEST_CASE(UsesChildren)
{
	BOOST_TEST(gt::uses_children<strsize>::value);
}

BOOST_AUTO_TEST_CASE(DecodeEmpty)
{
	gt::mutable_tree tr;

	strsize map = {{"hello", 42}};
	gt::decode(tr, map);

	BOOST_TEST(map.empty());
}

BOOST_AUTO_TEST_CASE(DecodeValueToGeneral)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 42 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 24 }}
		}}
	}};

	strsize map;
	gt::decode(tr, map);

	BOOST_TEST(map.size() == 2);
	BOOST_TEST(map["hello"] == 42);
	BOOST_TEST(map["world"] == 24);
}

BOOST_AUTO_TEST_CASE(DecodeValueToContainer)
{
	using svec = std::vector<std::size_t>;

	gt::mutable_tree tr{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 24 }},
			gt::mutable_tree{{ 42 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 42 }},
			gt::mutable_tree{{ 24 }}
		}},
	}};

	svec hello = { 24, 42 };
	svec world = { 42, 24 };

	std::map<std::string, svec> map;
	gt::decode(tr, map);

	BOOST_TEST(map.size() == 2);
	BOOST_TEST(map["hello"] == hello, tt::per_element());
	BOOST_TEST(map["world"] == world, tt::per_element());
}

BOOST_AUTO_TEST_CASE(DecodeContainerToValue)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 24 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 42 }}
		}},
	}};

	std::map<std::optional<std::size_t>, std::string> map;
	gt::decode(tr, map);

	BOOST_TEST(map.size() == 2);
	BOOST_TEST(map[24] == "hello");
	BOOST_TEST(map[42] == "world");
}

BOOST_AUTO_TEST_CASE(DecodeGeneralToGeneral)
{
	gt::mutable_tree tr{{
		gt::mutable_tree{{
			gt::mutable_tree{{0}, {
				gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }},
			}},
			gt::mutable_tree{{1}, {
				gt::mutable_tree{{ 42 }},
			}}
		}},
		gt::mutable_tree{{
			gt::mutable_tree{{0}, {
				gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }},
			}},
			gt::mutable_tree{{1}, {
				gt::mutable_tree{{ 24 }},
			}}
		}}
	}};

	using stint = std::variant<std::string, std::size_t>;
	std::map<stint, stint> map;
	gt::decode(tr, map);

	BOOST_TEST(map.size() == 2);
	BOOST_CHECK(map["hello"] == stint{42LU});
	BOOST_CHECK(map["world"] == stint{24LU});
}

BOOST_AUTO_TEST_CASE(EncodeEmpty)
{
	gt::mutable_tree tr{{ gt::mutable_tree{} }};

	strsize map;
	gt::encode(map, tr);

	BOOST_TEST(is_empty(tr));
}

BOOST_AUTO_TEST_CASE(EncodeValueToGeneral)
{
	gt::mutable_tree expect{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 42 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 24 }}
		}}
	}}, result;

	strsize map = {
		{ "hello", 42 },
		{ "world", 24 }
	};

	gt::encode(map, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(EncodeValueToContainer)
{
	using svec = std::vector<std::size_t>;

	gt::mutable_tree expect{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 24 }},
			gt::mutable_tree{{ 42 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 42 }},
			gt::mutable_tree{{ 24 }}
		}},
	}}, result;

	std::map<std::string, svec> map {
		{ "hello", { 24, 42 } },
		{ "world", { 42, 24 } }
	};
	gt::encode(map, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(EncodeContainerToValue)
{
	gt::mutable_tree result, expect{{
		gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }, {
			gt::mutable_tree{{ 24 }}
		}},
		gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }, {
			gt::mutable_tree{{ 42 }}
		}},
	}};

	std::map<std::optional<std::size_t>, std::string> map{
		{ 24, "hello" },
		{ 42, "world" }
	};
	gt::encode(map, result);

	BOOST_CHECK(result == expect);
}

BOOST_AUTO_TEST_CASE(EncodeGeneralToGeneral)
{
	gt::mutable_tree result, expect{{
		gt::mutable_tree{{
			gt::mutable_tree{{}, {
				gt::mutable_tree{{ 'h', 'e', 'l', 'l', 'o' }},
			}},
			gt::mutable_tree{{1}, {
				gt::mutable_tree{{ 42 }},
			}}
		}},
		gt::mutable_tree{{
			gt::mutable_tree{{}, {
				gt::mutable_tree{{ 'w', 'o', 'r', 'l', 'd' }},
			}},
			gt::mutable_tree{{1}, {
				gt::mutable_tree{{ 24 }},
			}}
		}}
	}};

	using stint = std::variant<std::string, std::size_t>;
	std::map<stint, stint> map{
		{ "hello", 42LU },
		{ "world", 24LU }
	};
	gt::encode(map, result);

	BOOST_CHECK(result == expect);
}
