#define BOOST_TEST_MODULE DynamicTreeTest
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>
#include <boost/test/data/monomorphic.hpp>
#include <boost/test/data/test_case.hpp>
namespace tt = boost::test_tools;
namespace bd = boost::unit_test::data;

#include "gulachek/gtree.hpp"

namespace gt = gulachek::gtree;

BOOST_AUTO_TEST_CASE(IsTree)
{
	BOOST_TEST(gt::is_tree<gt::dynamic_to_static>::value);
}

std::size_t child_count(const gt::dynamic_tree &tr)
{
	return tr.child_count();
}

BOOST_AUTO_TEST_CASE(AdaptChildCount)
{
	std::vector<unsigned int> v = {1, 2, 3};
	gt::mutable_tree tr;
	gt::encode(v, tr);

	gt::static_to_dynamic adapt{tr};

	BOOST_TEST(child_count(adapt) == 3);
}

const gt::block value(const gt::dynamic_tree &tr)
{
	return tr.value();
}

BOOST_AUTO_TEST_CASE(AdaptValue)
{
	std::size_t i = 3;
	gt::mutable_tree tr;
	gt::encode(i, tr);

	gt::static_to_dynamic adapt{tr};

	BOOST_TEST(*value(adapt).data() == 3);
}

std::shared_ptr<const gt::dynamic_tree> child(
		const gt::dynamic_tree &tr,
		std::size_t i
		)
{
	return tr.child(i);
}

BOOST_AUTO_TEST_CASE(AdaptChild)
{
	std::vector<std::size_t> v = {1, 2, 3};
	gt::mutable_tree tr;
	gt::encode(v, tr);

	gt::static_to_dynamic adapt{tr};

	auto two = value(*child(adapt, 1));
	BOOST_TEST(*two.data() == 2);
}

BOOST_AUTO_TEST_CASE(AdaptUsingMakeDynamic)
{
	std::vector<std::size_t> v = {1, 2, 3};
	gt::mutable_tree tr;
	gt::encode(v, tr);

	auto adapt = gt::make_dynamic<gt::static_to_dynamic<gt::mutable_tree>>(tr);

	auto two = value(*child(*adapt, 1));
	BOOST_TEST(*two.data() == 2);
}

BOOST_AUTO_TEST_CASE(AdaptUsingToDynamic)
{
	std::vector<std::size_t> v = {1, 2, 3};
	gt::mutable_tree tr;
	gt::encode(v, tr);

	auto adapt = gt::to_dynamic(tr);

	auto two = value(*child(*adapt, 1));
	BOOST_TEST(*two.data() == 2);
}

BOOST_AUTO_TEST_CASE(DynamicMadeStatic)
{
	std::vector<std::size_t> expect = {1, 2, 3}, actual;
	gt::mutable_tree tr;
	gt::encode(expect, tr);

	// we already know this works
	auto dynamic = std::make_shared<
		gt::static_to_dynamic<gt::mutable_tree>>(tr);

	gt::dynamic_to_static adapt{dynamic};

	gt::decode(adapt, actual);

	BOOST_TEST(actual == expect, tt::per_element());
}
