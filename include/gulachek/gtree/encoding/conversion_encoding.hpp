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
	struct encoding<T,
		enable_t<typename convert_encoding<T>::source>
			>
	{
		using type = T;
		using source_type = typename convert_encoding<T>::source;

		static constexpr bool uses_value = gtree::uses_value<source_type>::value;
		static constexpr bool uses_children = gtree::uses_children<source_type>::value;

		template <typename Tree>
		static error decode(Tree &&tr, type &val)
		{
			source_type src;

			if (auto err = gtree::decode(std::forward<Tree>(tr), src))
				return err;

			return convert_encoding<T>::decode(std::move(src), val);
		}

		template <typename ForwardRef, typename MutableTree>
		static error encode(ForwardRef &&val, MutableTree &tr)
		{
			source_type src;

			if (auto err = convert_encoding<T>::encode(std::forward<ForwardRef>(val), src))
				return err;

			return encode(std::move(src), tr);
		}
	};
}

#endif
