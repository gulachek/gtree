#ifndef GULACHEK_GTREE_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_HPP


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

#endif
