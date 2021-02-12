#ifndef GULACHEK_GTREE_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_HPP

#include "gulachek/gtree/is_itreem.hpp"
#include "gulachek/gtree/is_otreem.hpp"
#include "gulachek/gtree/mutable_tree.hpp"
#include "gulachek/gtree/encoding/encoding.hpp"
#include "gulachek/gtree/encoding/tree.hpp"
#include "gulachek/gtree/encoding/empty.hpp"
#include "gulachek/gtree/encoding/string.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"
#include "gulachek/gtree/encoding/signed.hpp"
#include "gulachek/gtree/encoding/enum.hpp"
#include "gulachek/gtree/encoding/vector.hpp"
#include "gulachek/gtree/encoding/variant.hpp"
#include "gulachek/gtree/encoding/optional.hpp"
#include "gulachek/gtree/encoding/fusion.hpp"
#include "gulachek/gtree/encoding/map.hpp"
#include "gulachek/gtree/encoding/polymorphic_variant.hpp"
#include "gulachek/gtree/encoding/set.hpp"
#include "gulachek/gtree/encoding/conversion_encoding.hpp"
#include "gulachek/gtree/encoding/dictionary.hpp"

#include <type_traits>
#include <string>
#include <list>
#include <cstdint>
#include <variant>
#include <optional>

namespace gulachek::gtree
{
	// ========================================
	// Conventional encodings
	// ========================================
	/*
	 * String (value)
	 *
	 * 	Simply string of bytes
	 * 	simplest type - pure data
	 *
	 * Unsigned Integer (value)
	 *
	 * 	Variable width
	 * 	little endian
	 *
	 * Signed Integer (value)
	 *
	 * 	Variable width
	 * 	little endian
	 * 	2s complement (high bit at byte boundary)
	 *
	 * Pair
	 *
	 * 	Associates 2 typed values together w/ optimization:
	 *
	 * 	
	 *
	 * Vector (container)
	 *
	 * 	Each child represents an element of same type
	 *
	 * Associative
	 *
	 * 	Maps elements of one type to another
	 *
	 * 	Unique keys
	 *
	 * 	Given key uses children:
	 * 		Vector of { Key Value }
	 * 	Else:
	 * 		Given value uses value:
	 * 			Vector of { Key | Value }
	 * 		else:
	 * 			Vector of { Key | ...Value }
	 *
	 * Optional
	 *
	 * 	Either an item exists or doesn't
	 *
	 * 	Equivalent to KVP of null => T
	 *
	 * 	{ Value } (given uses value)
	 * 	{ ...Value } (given does not use value)
	 *
	 * Variant
	 *
	 * 	Discrete/Bound set of types a single object can take
	 *
	 * 	Type identified by 0-based Unsigned index
	 *
	 * 	Equivalent to KVP Unsigned => T[i]
	 *
	 * Fraction ???
	 *
	 */

}

template <
	typename Itreem,
	typename T,
	std::enable_if_t<
		gulachek::gtree::is_itreem<Itreem>::value,
		int
		> = 0
	>
Itreem& operator >> (Itreem& it, T &t)
{
	typename Itreem::tree_type tree;
	if (it.read(&tree))
		gulachek::gtree::decode(tree, t);

	return it;
}

template <
	typename Otreem,
	typename T,
	std::enable_if_t<
		gulachek::gtree::is_otreem<Otreem>::value,
		int
		> = 0
	>
Otreem& operator << (Otreem& ot, const T &val)
{
	gulachek::gtree::mutable_tree tree;
	gulachek::gtree::encode(val, tree);
	ot.write(tree);
	return ot;
}

#endif
