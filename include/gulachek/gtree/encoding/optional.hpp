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

		cause decode(treeder &r)
		{
			if (r.child_count())
			{
				T val;
				if (auto err = r.read(&val))
				{
					cause wrap{"error reading optional value"};
					wrap.add_cause(err);
					return wrap;
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

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);

			if (o)
			{
				w.child_count(1);
				if (auto err = w.write(*o))
				{
					cause wrap{"error writing optional value"};
					wrap.add_cause(err);
					return wrap;
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
