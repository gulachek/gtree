#ifndef GULACHEK_GTREE_ENCODING_CONVERSION_ENCODING_HPP
#define GULACHEK_GTREE_ENCODING_CONVERSION_ENCODING_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

namespace gulachek::gtree
{
	/*
	 * Implement this if an encoding should be identical
	 * to another type.
	 * 
	 * using source = YOUR SOURCE TYPE (like int);
	 * error encode(T &&val, source &ref);
	 * error decode(source &&ref, T &val);
	 *
	 */
	template <typename T, typename Enable = void>
	struct convert_encoding {};

	template <typename T>
	struct uses_value<
		T,
		enable_t<typename convert_encoding<T>::source>
			> : uses_value<typename convert_encoding<T>::source> {};

	template <typename T>
	struct uses_children<
		T,
		enable_t<typename convert_encoding<T>::source>
			> : uses_children<
			typename convert_encoding<T>::source> {};

	template <
		typename Tree,
		typename T,
		typename convert_encoding<T>::source* = nullptr
						 >
	error decode(Tree &&tr, T &val)
	{
		typename convert_encoding<T>::source src;

		if (auto err = decode(tr, src))
			return err;

		return convert_encoding<T>::decode(std::move(src), val);
	}

	template <
		typename T,
		typename MutableTree,
		typename convert_encoding<T>::source* = nullptr
						 >
	error encode(T &&val, MutableTree &tr)
	{
		typename convert_encoding<T>::source src;

		if (auto err = convert_encoding<T>::encode(std::forward<T>(val), src))
			return err;

		return encode(std::move(src), tr);
	}
}

#endif
