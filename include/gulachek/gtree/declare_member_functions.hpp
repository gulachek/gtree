#ifndef GULACHEK_GTREE_DECLARE_MEMBER_FUNCTIONS_HPP
#define GULACHEK_GTREE_DECLARE_MEMBER_FUNCTIONS_HPP

#include <gulachek/error.hpp>
#include <gulachek/gtree/encoding.hpp>
#include <gulachek/gtree/decoding.hpp>

#define GTREE_DECLARE_MEMBER_FNS \
	gulachek::error gtree_decode(gulachek::gtree::treeder &r); \
	gulachek::error gtree_encode(gulachek::gtree::tree_writer &w) const;

#endif
