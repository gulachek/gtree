#ifndef GULACHEK_GTREE_ENCODING_OPTIONAL_HPP
#define GULACHEK_GTREE_ENCODING_OPTIONAL_HPP

#include "gulachek/gtree/encoding/encoding.hpp"
#include <optional>

namespace gulachek::gtree
{
	template <typename U>
	struct encoding<std::optional<U>>
	{
		using type = std::optional<U>;
		using value_type = typename type::value_type;

		static constexpr bool uses_value = false;
		static constexpr bool uses_children = true;

		template <typename Opt, typename Tree>
		static error encode(Opt &&val, Tree &tree)
		{
			if (val)
			{
				tree.child_count(1);
				return gtree::encode(*std::forward<Opt>(val), tree.child(0));
			}
			else
			{
				tree.child_count(0);
				return {};
			}
		}

		template <typename Tree>
		static error decode(Tree &&tree, type &val)
		{
			if (tree.child_count())
			{
				typename type::value_type inner;
				if (auto err = gtree::decode(std::forward<Tree>(tree).child(0), inner))
					return err;

				val = std::move(inner);
			}
			else
			{
				val = std::nullopt;
			}

			return {};
		}
	};
}

#endif
