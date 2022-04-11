#ifndef GULACHEK_GTREE_DECLARE_MEMBER_FUNCTIONS_HPP
#define GULACHEK_GTREE_DECLARE_MEMBER_FUNCTIONS_HPP

#include <gulachek/cause.hpp>
#include <gulachek/gtree/encoding.hpp>
#include <gulachek/gtree/decoding.hpp>

#define GTREE_DECLARE_MEMBER_FNS \
	gulachek::cause gtree_decode(gulachek::gtree::treeder &r); \
	gulachek::cause gtree_encode(gulachek::gtree::tree_writer &w) const;

#endif
