#ifndef GULACHEK_GTREE_ITREEM_HPP
#define GULACHEK_GTREE_ITREEM_HPP

#include "gulachek/gtree/gnatz_reader.hpp"
#include "gulachek/gtree/gtree_reader.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/gnatz_tree_builder.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <type_traits>
#include <istream>

namespace gulachek::gtree
{
	template <typename InStream>
	class basic_itreem
	{
		public:
			typedef tree tree_type;

			basic_itreem(InStream &is) :
				_is{is},
				_gnz{is},
				_builder{},
				_gtree{_gnz, _is, _builder},
				_good{true}
			{}

			bool read(tree *ptree)
			{
				if (!_gtree.read(ptree))
					_good = false;

				return _good;
			}

			operator bool()
			{ return _good; }

		private:
			InStream &_is;
			gnatz_reader<InStream> _gnz;
			gnatz_tree_builder _builder;
			gtree_reader<decltype(_gnz), InStream, decltype(_builder)> _gtree;
			bool _good;
	};

	typedef basic_itreem<std::istream> itreem;
}

template <typename InStream, typename T>
gulachek::gtree::basic_itreem<InStream>& operator >> (
		gulachek::gtree::basic_itreem<InStream>& it,
		T &t
		)
{
	gulachek::gtree::tree tree;
	if (it.read(&tree))
		gulachek::gtree::decode(tree, t);

	return it;
}

#endif
