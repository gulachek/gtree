#include "gulachek/gtree/itreem.hpp"
#include "gulachek/gtree/basic_itreem.hpp"

#include <iostream>

namespace gulachek::gtree
{
	struct itreem::impl
	{
		basic_itreem<std::istream> i;

		impl(std::istream &is) :
			i{is}
		{}

		impl(const itreem::impl&) = delete;
	};

	itreem::itreem(std::istream &s) :
		_pimpl{std::make_unique<itreem::impl>(s)}
	{}

	itreem::~itreem() {}

	itreem::operator bool()
	{
		return (bool)_pimpl->i;
	}

	bool itreem::read(tree *ptree)
	{
		return _pimpl->i.read(ptree);
	}
}
