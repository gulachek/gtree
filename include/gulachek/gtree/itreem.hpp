#ifndef GULACHEK_GTREE_ITREEM_HPP
#define GULACHEK_GTREE_ITREEM_HPP

#include "gulachek/gtree/is_itreem.hpp"
#include "gulachek/gtree/tree.hpp"

#include <istream>
#include <memory>

namespace gulachek::gtree
{
	// wrapper for easier compilation - perhaps something
	// clever with explicit instantiation could be done
	// as typedef but this works for now
	class itreem
	{
		public:
			typedef tree tree_type;
			itreem(std::istream &is);
			~itreem();
			bool read(tree *ptree);
			operator bool();

		private:
			struct impl;
			std::unique_ptr<impl> _pimpl;
	};

	template <>
	struct is_itreem<itreem> : std::true_type {};
}

#endif
