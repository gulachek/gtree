#ifndef TEST_SABOTEUR_HPP
#define TEST_SABOTEUR_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

struct saboteur
{
	bool error = false;

	gulachek::error gtree_decode(gulachek::gtree::treeder &r)
	{
		auto val = r.value();
		if (val.empty())
			return {};
		else
			return {"saboteur has value"};
	}

	gulachek::error gtree_encode(gulachek::gtree::tree_writer &w) const
	{
		w.value(nullptr, 0);
		w.child_count(0);
		if (error)
			return {"saboteur has error"};
		else
			return {};
	}
};

#endif
