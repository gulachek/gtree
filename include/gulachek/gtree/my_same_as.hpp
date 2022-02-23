#ifndef GULACHEK_GTREE_MY_SAME_AS_HPP
#define GULACHEK_GTREE_MY_SAME_AS_HPP

#include <type_traits>

namespace gulachek::gtree
{
	template <typename T, typename U>
	concept my_same_as = std::is_same_v<T, U>;
}

#endif
