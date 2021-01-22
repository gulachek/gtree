#ifndef GULACHEK_GTREE_HPP
#define GULACHEK_GTREE_HPP

#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/dynamic_tree.hpp"
#include "gulachek/gtree/itreem.hpp"
#include "gulachek/gtree/otreem.hpp"

#include <iostream>

namespace gulachek::gtree
{
	extern otreem<decltype(std::cout)> tout;
	extern itreem tin;
}

#endif
