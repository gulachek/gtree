#ifndef GULACHEK_GTREE_ENCODING_SHARED_POINTER_HPP
#define GULACHEK_GTREE_ENCODING_SHARED_POINTER_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include <memory>

namespace gulachek::gtree
{
	template <typename T>
	struct encoding<std::shared_ptr<T>>
	{
		using type = std::shared_ptr<T>;
		using value_type = T;

		static constexpr bool uses_value = gtree::uses_value<value_type>::value;
		static constexpr bool uses_children = gtree::uses_children<value_type>::value;

		template <typename Tree>
		static error decode(Tree &&tr, type &val)
		{
			value_type src;

			if (auto err = gtree::decode(std::forward<Tree>(tr), src))
				return err;

			val = std::make_shared<value_type>(std::move(src));
			return {};
		}

		template <typename Ptr, typename MutableTree>
		static error encode(Ptr &&val, MutableTree &tr)
		{
			// use lvalue ref because this might be a shared value
			return gtree::encode(*val, tr);
		}
	};

}

#endif
