#ifndef GULACHEK_GTREE_ENCODING_LIST_HPP
#define GULACHEK_GTREE_ENCODING_LIST_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <list>

namespace gulachek::gtree
{
	template <decodable T, typename Allocator>
	struct decoding<std::list<T, Allocator>>
	{
		std::list<T, Allocator> *l;

		cause decode(treeder &r)
		{
			auto n = r.child_count();
			l->clear();

			for (std::size_t i = 0; i < n; ++i)
			{
				T elem;
				if (auto err = r.read(&elem))
				{
					cause wrapper;
					wrapper << "error decoding list element " << i;
					wrapper.add_cause(err);
					return wrapper;
				}

				l->emplace_back(std::move(elem));
			}

			return {};
		}
	};

	template <encodable T, typename Allocator>
	struct encoding<std::list<T, Allocator>>
	{
		const std::list<T, Allocator> &l;

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(l.size());

			std::size_t i = 0;
			for (const auto &elem : l)
			{
				if (auto err = w.write(elem))
				{
					cause wrapper;
					wrapper << "error encoding list element " << i;
					wrapper.add_cause(err);
					return wrapper;
				}
				++i;
			}

			return {};
		}
	};
}

#endif
