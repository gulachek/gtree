#ifndef GULACHEK_GTREE_ENCODING_VECTOR_HPP
#define GULACHEK_GTREE_ENCODING_VECTOR_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <vector>

namespace gulachek::gtree
{
	template <decodable T, typename Allocator>
	struct decoding<std::vector<T, Allocator>>
	{
		std::vector<T, Allocator> *v;

		error decode(treeder &r)
		{
			auto n = r.child_count();
			v->resize(n);

			for (std::size_t i = 0; i < n; ++i)
			{
				if (auto err = r.read(&v->at(i)))
				{
					return err.wrap() << "error decoding vector element " << i;
				}
			}

			return {};
		}
	};

	template <encodable T, typename Allocator>
	struct encoding<std::vector<T, Allocator>>
	{
		const std::vector<T, Allocator> &v;

		error encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(v.size());

			for (std::size_t i = 0; i < v.size(); ++i)
			{
				if (auto err = w.write(v[i]))
				{
					return err.wrap() << "error encoding vector element " << i;
				}
			}

			return {};
		}
	};
}

#endif
