#include "gulachek/gtree/block.hpp"

using namespace gulachek::gtree;

bool operator == (const block &left, const block &right)
{
	if (left.size() != right.size())
		return false;

	if (left.data() == right.data())
		return true;

	for (std::size_t i = 0; i < left.size(); i++)
		if (left[i] != right[i])
			return false;

	return true;
}

bool operator != (const block &left, const block &right)
{
	return !(left == right);
}

std::string std::to_string(
		const gulachek::gtree::block &block)
{
	return std::string{
		block.data(), block.data() + block.size()};
}

std::ostream& operator << (std::ostream& os,
		const gulachek::gtree::block &b)
{
	return os << std::to_string(b);
}
