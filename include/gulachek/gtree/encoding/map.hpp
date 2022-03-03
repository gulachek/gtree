#ifndef GULACHEK_GTREE_ENCODING_MAP_HPP
#define GULACHEK_GTREE_ENCODING_MAP_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/encoding/pair.hpp"

#include <map>

namespace gulachek::gtree
{
	template <typename K, typename V, typename C, typename A>
	struct decoding<std::map<K,V,C,A>>
	{
		std::map<K,V,C,A> *pm;

		cause decode(treeder &r)
		{
			auto n = r.child_count();
			for (std::size_t i = 0; i < n; ++i)
			{
				std::pair<K,V> p;
				if (auto err = r.read(&p))
				{
					cause wrap{"error reading map elem"};
					wrap.add_cause(err);
					return wrap;
				}

				auto it_new = pm->insert(std::move(p));
				if (!it_new.second)
				{
					cause err{"error decoding map due to duplicate key"};
					if constexpr (cause_writable<K>)
					{
						err << ": " << it_new.first->first;
					}
					return err;
				}
			}

			return {};
		}
	};

	template <typename K, typename V, typename C, typename A>
	struct encoding<std::map<K,V,C,A>>
	{
		const std::map<K,V,C,A> &m;

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(m.size());

			for (const auto &elem : m)
			{
				if (auto err = w.write(elem))
				{
					cause wrap{"error writing map elem"};
					if constexpr (cause_writable<K>)
					{
						wrap << " with key " << elem.first;
					}
					wrap.add_cause(err);
					return wrap;
				}
			}

			return {};
		}
	};
}

#endif
