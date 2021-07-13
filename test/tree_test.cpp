#define BOOST_TEST_MODULE TreeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree/tree.hpp"
#include <type_traits>

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_SUITE(TypeTraits)

BOOST_AUTO_TEST_CASE(MoveConstructible)
{
	auto can_move = std::is_move_constructible<gt::tree>::value;
	BOOST_TEST(can_move);
}

BOOST_AUTO_TEST_CASE(CopyConstructible)
{
	auto can_copy = std::is_copy_constructible<gt::tree>::value;
	BOOST_TEST(can_copy);
}

BOOST_AUTO_TEST_SUITE(IsTreeTrait)

struct basic_tree {
	const gt::block value() const;
	std::size_t child_count() const;
	basic_tree child(std::size_t) const;
};

struct mutable_value : basic_tree
{
	const gt::block value() const { return basic_tree::value(); }

	template <typename T>
	void value(T begin, T end);
};

BOOST_AUTO_TEST_CASE(NoConflictWithMutableValue)
{
	BOOST_TEST(gt::is_tree<mutable_value>::value);
}

struct mutable_child_count : basic_tree
{
	std::size_t child_count() const
	{ return basic_tree::child_count(); }

	void child_count(std::size_t i);
};

BOOST_AUTO_TEST_CASE(NoConflictWithMutableChildCount)
{
	BOOST_TEST(gt::is_tree<mutable_child_count>::value);
}

struct MutableChild : basic_tree
{
	basic_tree child(std::size_t i) const;

	basic_tree& child(std::size_t i);
};

BOOST_AUTO_TEST_CASE(NoConflictWithMutableChild)
{
	BOOST_TEST(gt::is_tree<MutableChild>::value);
}

BOOST_AUTO_TEST_CASE(basic_treeIsTree)
{
	BOOST_TEST(gt::is_tree<basic_tree>::value);
}

BOOST_AUTO_TEST_CASE(NoDefaultConstructorIsNotTree)
{
	struct no_default_constructor : basic_tree {
		no_default_constructor(int i);
	};

	BOOST_TEST(!gt::is_tree<no_default_constructor>::value);
}

BOOST_AUTO_TEST_CASE(NoMoveConstructorIsNotTree)
{
	struct NoMoveConstructor : basic_tree {
		NoMoveConstructor() = default;
		NoMoveConstructor(NoMoveConstructor &&) = delete;
	};

	BOOST_TEST(!gt::is_tree<NoMoveConstructor>::value);
}

struct TreeNoValue
{
	std::size_t child_count() const;
	basic_tree child(std::size_t) const;
};

BOOST_AUTO_TEST_CASE(NoValueIsNotTree)
{
	BOOST_TEST(!gt::is_tree<TreeNoValue>::value);
}

BOOST_AUTO_TEST_CASE(ValueNotBlockIsNotTree)
{
	struct ValueNotBlock : TreeNoValue
	{
		int value() const;
	};

	BOOST_TEST(!gt::is_tree<ValueNotBlock>::value);
}

BOOST_AUTO_TEST_CASE(ValueNotConstBlockIsNotTree)
{
	struct ValueNotConstBlock : TreeNoValue
	{
		gt::block value() const;
	};

	BOOST_TEST(!gt::is_tree<ValueNotConstBlock>::value);
}

BOOST_AUTO_TEST_CASE(ValueNotConstMethodIsNotTree)
{
	struct ValueNotConstMethod : TreeNoValue
	{
		const gt::block value();
	};

	BOOST_TEST(!gt::is_tree<ValueNotConstMethod>::value);
}

BOOST_AUTO_TEST_CASE(ValueTakesParametersIsNotTree)
{
	struct ValueTakesParameters : TreeNoValue
	{
		const gt::block value(int i) const;
	};

	BOOST_TEST(!gt::is_tree<ValueTakesParameters>::value);
}

BOOST_AUTO_TEST_CASE(PrivateValueIsNotTree)
{
	class PrivateValue : public TreeNoValue
	{
		const gt::block value() const;
	};

	BOOST_TEST(!gt::is_tree<PrivateValue>::value);
}

struct TreeNoChildCount
{
	const gt::block value() const;
	basic_tree child(std::size_t i) const;
};

BOOST_AUTO_TEST_CASE(NoChildCountIsNotTree)
{
	BOOST_TEST(!gt::is_tree<TreeNoChildCount>::value);
}

BOOST_AUTO_TEST_CASE(ChildCountNotSizeIsNotTree)
{
	struct ChildCountNotSize : TreeNoChildCount
	{
		int child_count() const;
	};

	BOOST_TEST(!gt::is_tree<ChildCountNotSize>::value);
}

BOOST_AUTO_TEST_CASE(ChildCountConstSizeIsTree)
{
	struct ChildCountConstSize : TreeNoChildCount
	{
		const std::size_t child_count() const;
	};

	BOOST_TEST(gt::is_tree<ChildCountConstSize>::value);
}

BOOST_AUTO_TEST_CASE(ChildCountNotConstMethodIsNotTree)
{
	struct ChildCountNotConstMethod : TreeNoChildCount
	{
		std::size_t child_count();
	};

	BOOST_TEST(!gt::is_tree<ChildCountNotConstMethod>::value);
}

BOOST_AUTO_TEST_CASE(ChildCountTakesParametersIsNotTree)
{
	struct ChildCountTakesParameters : TreeNoChildCount
	{
		std::size_t child_count(int i) const;
	};

	BOOST_TEST(!gt::is_tree<ChildCountTakesParameters>::value);
}

BOOST_AUTO_TEST_CASE(PrivateChildCountIsNotTree)
{
	class PrivateChildCount : public TreeNoChildCount
	{
		std::size_t child_count() const;
	};

	BOOST_TEST(!gt::is_tree<PrivateChildCount>::value);
}

struct TreeNoChild
{
	const gt::block value() const;
	std::size_t child_count() const;
};

BOOST_AUTO_TEST_CASE(NoChildIsNotTree)
{
	BOOST_TEST(!gt::is_tree<TreeNoChild>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsSelfIsTree)
{
	struct ChildIsSelf : TreeNoChild
	{
		ChildIsSelf child(std::size_t) const;
	};

	BOOST_TEST(gt::is_tree<ChildIsSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsCrefSelfIsTree)
{
	struct ChildIsCrefSelf : TreeNoChild
	{
		const ChildIsCrefSelf& child(std::size_t) const;
	};

	BOOST_TEST(gt::is_tree<ChildIsCrefSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsRefSelfIsNotTree)
{
	struct ChildIsRefSelf : TreeNoChild
	{
		ChildIsRefSelf& child(std::size_t) const;
	};

	BOOST_TEST(!gt::is_tree<ChildIsRefSelf>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsOtherTreeIsTree)
{
	struct ChildIsOtherTree : TreeNoChild
	{
		basic_tree child(std::size_t) const;
	};

	BOOST_TEST(gt::is_tree<ChildIsOtherTree>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsCrefOtherIsTree)
{
	struct ChildIsCrefOther : TreeNoChild
	{
		const basic_tree& child(std::size_t) const;
	};

	BOOST_TEST(gt::is_tree<ChildIsCrefOther>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsRefOtherIsNotTree)
{
	struct ChildIsRefOther : TreeNoChild
	{
		basic_tree& child(std::size_t) const;
	};

	BOOST_TEST(!gt::is_tree<ChildIsRefOther>::value);
}

BOOST_AUTO_TEST_CASE(ChildIsNotConstMethodIsNotTree)
{
	struct ChildNotConstMethod : TreeNoChild
	{
		ChildNotConstMethod child(std::size_t);
	};

	BOOST_TEST(!gt::is_tree<ChildNotConstMethod>::value);
}

BOOST_AUTO_TEST_SUITE_END() // IsTreeTrait

BOOST_AUTO_TEST_CASE(CanonicalTreeIsTree)
{
	BOOST_TEST(gt::is_tree<gt::tree>::value);
}

BOOST_AUTO_TEST_SUITE_END() // TypeTraits

BOOST_AUTO_TEST_SUITE(Value)

BOOST_AUTO_TEST_CASE(MatchesValueOfTreeBase)
{
	std::uint8_t a = 'a', b = 'b';

	gt::tree_base base;
	base.nodes = {
		{{&a, 1}, 1, 1},
		{{&b, 1}, 0, 0}
	};

	gt::tree tree{std::make_shared<gt::tree_base>(std::move(base)), 0};

	auto val = tree.value();
	BOOST_TEST(val.data() == &a);
	BOOST_TEST(val.size() == 1);
}

BOOST_AUTO_TEST_CASE(DefaultValueIsEmpty)
{
	gt::tree tree;
	BOOST_TEST(tree.value().data() == nullptr);
	BOOST_TEST(tree.value().size() == 0);
}

BOOST_AUTO_TEST_SUITE_END() // Value

BOOST_AUTO_TEST_SUITE(ChildCount)

BOOST_AUTO_TEST_CASE(ChildCountSameAsTreeBase)
{
	constexpr int count = 25;

	std::uint8_t a = 'a';

	gt::tree_base base;
	base.nodes = { {{&a, 1}, 1, count} };

	gt::tree tree{std::make_shared<gt::tree_base>(std::move(base)), 0};

	BOOST_TEST(tree.child_count() == count);
}

BOOST_AUTO_TEST_CASE(DefaultNoChildren)
{
	gt::tree tree;
	BOOST_TEST(tree.child_count() == 0);
}

BOOST_AUTO_TEST_SUITE_END() // ChildCount

BOOST_AUTO_TEST_SUITE(Child)

BOOST_AUTO_TEST_CASE(ChildIndexedInTreeBase)
{
	std::uint8_t a = 'a', b = 'b';

	gt::tree_base base;
	base.nodes = {
		{{&a, 1}, 2, 1},
		{{&a, 1}, 3, 2},
		{{&a, 1}, 0, 0},
		{{&a, 1}, 0, 0},
		{{&b, 1}, 0, 0}
	};

	gt::tree tree{std::make_shared<gt::tree_base>(std::move(base)), 1};

	BOOST_TEST(tree.child(0).value().data() == &a);
	BOOST_TEST(tree.child(1).value().data() == &b);
}

BOOST_AUTO_TEST_SUITE_END() // Child
