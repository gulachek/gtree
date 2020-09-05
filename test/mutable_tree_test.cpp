#define BOOST_TEST_MODULE mutable_treeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/mutable_tree.hpp"
#include <type_traits>

namespace gt = gulachek::gtree;

struct Basicmutable_tree
{
	template <typename Iterator>
	void value(Iterator first, Iterator last);

	const gt::block value() const;

	void child_count(std::size_t);
	std::size_t child_count() const;

	Basicmutable_tree& child(std::size_t);
	Basicmutable_tree child(std::size_t) const;
};

struct ContrivedNotTree : Basicmutable_tree {};
template <>
struct gt::is_tree<ContrivedNotTree> : std::false_type {};

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(CanonicalIsmutable_tree)
{
	BOOST_TEST(gt::is_mutable_tree<gt::mutable_tree>::value);
}

BOOST_AUTO_TEST_SUITE(Ismutable_tree)

BOOST_AUTO_TEST_CASE(BasicIsMT)
{
	BOOST_TEST(gt::is_mutable_tree<Basicmutable_tree>::value);
}

BOOST_AUTO_TEST_CASE(MustBeIsTree)
{
	BOOST_TEST(!gt::is_mutable_tree<ContrivedNotTree>::value);
}

struct NoValue
{
	const gt::block value() const;

	void child_count(std::size_t);
	std::size_t child_count() const;

	Basicmutable_tree& child(std::size_t);
	Basicmutable_tree child(std::size_t) const;
};

BOOST_AUTO_TEST_CASE(NoValueIsNotMT)
{
	BOOST_TEST(!gt::is_mutable_tree<NoValue>::value);
}

struct ValueConstMethod : NoValue
{
	template <typename Iterator>
	void value(Iterator, Iterator) const;
};

BOOST_AUTO_TEST_CASE(ValueConstMethodIsNotMT)
{
	BOOST_TEST(!gt::is_mutable_tree<ValueConstMethod>::value);
}

struct NoChildCount
{
	template <typename Iterator>
	void value(Iterator first, Iterator last);
	const gt::block value() const;

	std::size_t child_count() const;

	Basicmutable_tree& child(std::size_t);
	Basicmutable_tree child(std::size_t) const;
};

BOOST_AUTO_TEST_CASE(NoChildCountIsNotMT)
{
	BOOST_TEST(!gt::is_mutable_tree<NoChildCount>::value);
}

BOOST_AUTO_TEST_CASE(ConstChildCountMethodIsNotMT)
{
	struct ConstChildCount : NoChildCount
	{
		void child_count(std::size_t) const;
	};

	BOOST_TEST(!gt::is_mutable_tree<ConstChildCount>::value);
}

struct NoChild
{
	template <typename Iterator>
	void value(Iterator first, Iterator last);
	const gt::block value() const;

	void child_count(std::size_t);
	std::size_t child_count() const;

	Basicmutable_tree child(std::size_t) const;
};

BOOST_AUTO_TEST_CASE(NoChildIsNotMT)
{
	BOOST_TEST(!gt::is_mutable_tree<NoChild>::value);
}

BOOST_AUTO_TEST_CASE(ChildConstRefToSelfIsNotMT)
{
	struct ChildCrefSelf : NoChild
	{
		const ChildCrefSelf& child(std::size_t);
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildCrefSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildValueToSelfIsNotMT)
{
	struct ChildValueSelf : NoChild
	{
		ChildValueSelf child(std::size_t);
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildValueSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildRefToConstSelfIsNotMT)
{
	struct ChildRefConstSelf : NoChild
	{
		ChildRefConstSelf& child(std::size_t) const;
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildRefConstSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildConstRefToOtherIsNotMT)
{
	struct ChildCrefOther : NoChild
	{
		const Basicmutable_tree& child(std::size_t);
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildCrefOther>::value);
}

BOOST_AUTO_TEST_CASE(ChildValueToOtherIsNotMT)
{
	struct ChildValueOther : NoChild
	{
		Basicmutable_tree child(std::size_t);
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildValueOther>::value);
}

BOOST_AUTO_TEST_CASE(ChildRefToConstOtherIsNotMT)
{
	struct ChildRefConstOther : NoChild
	{
		Basicmutable_tree& child(std::size_t) const;
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildRefConstOther>::value);
}

BOOST_AUTO_TEST_CASE(ChildRefNonMTIsNotMT)
{
	struct ChildRefNonMT : NoChild
	{
		int& child(std::size_t);
	};

	BOOST_TEST(!gt::is_mutable_tree<ChildRefNonMT>::value);
}

BOOST_AUTO_TEST_CASE(ChildRefOtherMTIsMT)
{
	struct ChildRefMT : NoChild
	{
		Basicmutable_tree& child(std::size_t);
		const Basicmutable_tree& child(std::size_t) const;
	};

	BOOST_REQUIRE(gt::is_tree<ChildRefMT>::value);
	BOOST_TEST(gt::is_mutable_tree<ChildRefMT>::value);
}

BOOST_AUTO_TEST_SUITE_END() // Ismutable_tree

BOOST_AUTO_TEST_SUITE_END() // TypeTraits

BOOST_AUTO_TEST_SUITE(Value)

BOOST_AUTO_TEST_CASE(DefaultValueIsEmpty)
{
	gt::mutable_tree tree;
	BOOST_TEST(is_empty(tree));
}

BOOST_AUTO_TEST_CASE(AssignedValuePersists)
{
	std::vector<std::uint8_t> value = { 1, 2, 3 };

	gt::mutable_tree left{ value };
	gt::mutable_tree right;

	right.value(value.begin(), value.end());

	auto size = right.value().size();
	BOOST_TEST(size == 3);
	bool same = left == right;
	BOOST_TEST(same);
}

BOOST_AUTO_TEST_SUITE_END() // Value

BOOST_AUTO_TEST_SUITE(ChildCount)

BOOST_AUTO_TEST_CASE(DefaultNoChildren)
{
	gt::mutable_tree tree;
	BOOST_TEST(tree.child_count() == 0);
}

BOOST_AUTO_TEST_CASE(CanResize)
{
	gt::mutable_tree tree;

	tree.child_count(3);

	BOOST_TEST(tree.child_count() == 3);
}

BOOST_AUTO_TEST_SUITE_END() // ChildCount

BOOST_AUTO_TEST_SUITE(Child)

BOOST_AUTO_TEST_CASE(ChildIsMutable)
{
	gt::mutable_tree left{{
		gt::mutable_tree{ { 'a', 'b', 'c' } },
		gt::mutable_tree{ { 1, 2, 3 } }
	}};

	gt::mutable_tree right;
	right.child_count(2);
	right.child(0) = left.child(0);
	right.child(1) = left.child(1);

	bool same = left == right;
	BOOST_CHECK(same);
}

BOOST_AUTO_TEST_SUITE_END() // Child
