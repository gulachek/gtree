#ifndef GULACHEK_GTREE_BASIC_ITREEM_HPP
#define GULACHEK_GTREE_BASIC_ITREEM_HPP

#include "gulachek/gtree/is_itreem.hpp"
#include "gulachek/gtree/gnatz_reader.hpp"
#include "gulachek/gtree/gtree_reader.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/gnatz_tree_builder.hpp"

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
				_gtree{_gnz, is, _builder},
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

	template <typename InStream>
	struct is_itreem<basic_itreem<InStream>> :
		std::true_type {};
}

#endif
