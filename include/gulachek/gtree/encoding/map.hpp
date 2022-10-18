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

		error decode(treeder &r)
		{
			auto n = r.child_count();
			for (std::size_t i = 0; i < n; ++i)
			{
				std::pair<K,V> p;
				if (auto err = r.read(&p))
				{
					return err.wrap() << "error reading map elem at index " << i;
				}

				auto it_new = pm->insert(std::move(p));
				if (!it_new.second)
				{
					error err{"error decoding map due to duplicate key"};
					if constexpr (error_writable<K>)
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

		error encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(m.size());

			for (const auto &elem : m)
			{
				if (auto err = w.write(elem))
				{
					error wrap{"error writing map elem"};
					if constexpr (error_writable<K>)
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
