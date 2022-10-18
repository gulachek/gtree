#ifndef GULACHEK_GTREE_ENCODING_FUNCTION_HPP
#define GULACHEK_GTREE_ENCODING_FUNCTION_HPP

// useful for inversion of control

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/my_same_as.hpp"

#include <vector>

namespace gulachek::gtree
{
	template <typename T>
	concept decode_invocable = requires (T &f, treeder &r)
	{
		{ f(r) } -> my_same_as<error>;
	};

	template <decode_invocable F>
	struct decoding<F>
	{
		F *f;

		error decode(treeder &r)
		{
			return (*f)(r);
		}
	};

	template <typename T>
	concept encode_invocable = requires (T &f, tree_writer &w)
	{
		{ f(w) } -> my_same_as<error>;
	};

	template <encode_invocable F>
	struct encoding<F>
	{
		const F &f;

		error encode(tree_writer &w)
		{
			return f(w);
		}
	};
}

#endif
