#ifndef GULACHEK_GTREE_ENDODING_SET_HPP
#define GULACHEK_GTREE_ENDODING_SET_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <set>

namespace gulachek::gtree
{
	template <typename K, typename C, typename A>
	struct decoding<std::set<K, C, A>>
	{
		std::set<K,C,A> *ps;

		cause decode(treeder &r)
		{
			auto n = r.child_count();
			for (std::size_t i = 0; i < n; ++i)
			{
				K elem;
				if (auto err = r.read(&elem))
				{
					cause wrap{"error reading set elem"};
					wrap.add_cause(err);
					return wrap;
				}
				ps->emplace(std::move(elem));
			}
			return {};
		}
	};

	template <typename K, typename C, typename A>
	struct encoding<std::set<K,C,A>>
	{
		const std::set<K,C,A> &s;

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(s.size());

			for (const auto &elem : s)
			{
				if (auto err = w.write(elem))
				{
					cause wrap{"error writing set elem"};
					wrap.add_cause(err);
					return wrap;
				}
			}

			return {};
		}
	};
}

#endif
