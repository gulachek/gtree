#ifndef GULACHEK_GTREE_ENCODING_SHARED_PTR_HPP
#define GULACHEK_GTREE_ENCODING_SHARED_PTR_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <memory>

namespace gulachek::gtree
{
	template <typename T>
	struct decoding<std::shared_ptr<T>>
	{
		std::shared_ptr<T> *pp;

		cause decode(treeder &r)
		{
			T val;
			decoding<T> dec{&val};
			if (auto err = dec.decode(r))
				return err;

			*pp = std::make_shared<T>(std::move(val));
			return {};
		}
	};

	template <typename T>
	struct encoding<std::shared_ptr<T>>
	{
		const std::shared_ptr<T> &p;

		cause encode(tree_writer &w)
		{
			if (!p)
				return {"cannot encode nullptr for std::shared_ptr"};

			return w.write(*p);
		}
	};
}

#endif
