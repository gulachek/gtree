#ifndef GULACHEK_GTREE_ENCODING_SHARED_POINTER_HPP
#define GULACHEK_GTREE_ENCODING_SHARED_POINTER_HPP

#include "gulachek/gtree/encoding/conversion_encoding.hpp"
#include <memory>

namespace gulachek::gtree
{
	template <typename T>
	struct convert_encoding<std::shared_ptr<T>>
	{
		using source = T;
		using ptr = std::shared_ptr<source>;

		static error encode(ptr &&p, source &ref)
		{
			ref = std::move(*p);
			return {};
		}

		static error decode(source &&ref, ptr &p)
		{
			p = std::make_shared<source>(std::forward<source>(ref));
			return {};
		}
	};

	/*
	template <typename T>
	struct convert_encoding<std::unique_ptr<T>>
	{
		using source = T;
		using ptr = std::unique_ptr<T>;

		static const T& encode(const ptr &p)
		{
			return *p;
		}

		static ptr decode(T &&src)
		{
			return std::make_unique<T>(std::forward<T>(src));
		}
	};
	*/
}

#endif
