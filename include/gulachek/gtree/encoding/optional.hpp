#ifndef GULACHEK_GTREE_ENCODING_OPTIONAL_HPP
#define GULACHEK_GTREE_ENCODING_OPTIONAL_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"
#include <optional>

namespace gulachek::gtree
{
	template <typename T>
	struct decoding<std::optional<T>>
	{
		std::optional<T> *p;

		error decode(treeder &r)
		{
			if (r.child_count())
			{
				T val;
				if (auto err = r.read(&val))
				{
					return err.wrap() << "error reading optional value";
				}
				p->emplace(std::move(val));
				return {};
			}
			else
			{
				*p = std::nullopt;
				return {};
			}
		}
	};

	template <typename T>
	struct encoding<std::optional<T>>
	{
		const std::optional<T> &o;

		error encode(tree_writer &w)
		{
			w.value(nullptr, 0);

			if (o)
			{
				w.child_count(1);
				if (auto err = w.write(*o))
				{
					return err.wrap() << "error writing optional value";
				}
			}
			else
			{
				w.child_count(0);
			}

			return {};
		}
	};
}

#endif
