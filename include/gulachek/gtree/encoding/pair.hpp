#ifndef GULACHEK_GTREE_ENCODING_PAIR_HPP
#define GULACHEK_GTREE_ENCODING_PAIR_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <utility>

namespace gulachek::gtree
{
	template <typename T, typename U>
	struct decoding<std::pair<T, U>>
	{
		std::pair<T, U> *p;

		cause decode(treeder &r)
		{
			if (r.child_count() < 2)
			{
				cause err;
				err << "not enough children to decode pair. child_count: " <<
					r.child_count();
				return err;
			}

			if (auto err = r.read(&p->first))
			{
				cause wrap{"error reading first pair elem"};
				wrap.add_cause(err);
				return wrap;
			}

			if (auto err = r.read(&p->second))
			{
				cause wrap{"error reading second pair elem"};
				wrap.add_cause(err);
				return wrap;
			}

			return {};
		}
	};

	template <typename T, typename U>
	struct encoding<std::pair<T, U>>
	{
		const std::pair<T, U> &p;

		cause encode(tree_writer &w)
		{
			w.value(nullptr, 0);
			w.child_count(2);
			
			if (auto err = w.write(p.first))
			{
				cause wrap{"error writing first pair elem"};
				wrap.add_cause(err);
				return wrap;
			}

			if (auto err = w.write(p.second))
			{
				cause wrap{"error writing second pair elem"};
				wrap.add_cause(err);
				return wrap;
			}

			return {};
		}
	};

	/*
	template <typename K, typename V>
	struct encoding<std::pair<K,V>>
	{
		using type = std::pair<K,V>;

		template <typename Tree>
		static error decode(Tree &&tree, type &pair)
		{
			if (tree.child_count() < 2)
				return {"expected 2 children for pair"};

			if (auto err =
					gtree::decode(std::forward<Tree>(tree).child(0),
						pair.first
						)
				 )
			{
				return err;
			}

			return gtree::decode(std::forward<Tree>(tree).child(1),
					pair.second);
		}

		template <typename Pair, typename MutableTree>
		static error encode(Pair &&pair, MutableTree &tree)
		{
			tree.child_count(2);
			if (auto err = gtree::encode(std::forward<Pair>(pair).first,
						tree.child(0)))
			{
				return err;
			}

			return gtree::encode(std::forward<Pair>(pair).second,
					tree.child(1));
		}
	};
	*/
}

#endif
