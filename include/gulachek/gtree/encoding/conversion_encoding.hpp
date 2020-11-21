#ifndef GULACHEK_GTREE_ENCODING_CONVERT_HPP
#define GULACHEK_GTREE_ENCODING_CONVERT_HPP

namespace gulachek::gtree
{
	/*
	 * Implement this if an encoding should be identical
	 * to another type.
	 * 
	 * using source = YOUR SOURCE TYPE (like int);
	 * const source& encode(const T &val);
	 * const T& decode(source &&src);
	 *
	 * the const ref return values can also be plain values
	 *
	 */
	template <typename T, typename Enable = void>
	struct convert_encoding {};

	template <typename T>
	struct __vs { using type = void*; };

	template <typename T>
	struct uses_value<
		T,
		typename __vs<typename convert_encoding<T>::source>::type
			> : uses_value<typename convert_encoding<T>::source> {};

	template <typename T>
	struct uses_children<
		T,
		typename __vs<typename convert_encoding<T>::source>::type
			> : uses_children<
			typename convert_encoding<T>::source> {};

	template <
		typename Tree,
		typename T,
		typename convert_encoding<T>::source* = nullptr
						 >
	void decode(const Tree &tr, T &val)
	{
		typename convert_encoding<T>::source src;
		decode(tr, src);
		val = convert_encoding<T>::decode(std::move(src));
	}

	template <
		typename T,
		typename MutableTree,
		typename convert_encoding<T>::source* = nullptr
						 >
	void encode(const T &val, MutableTree &tr)
	{
		const typename convert_encoding<T>::source &src =
			convert_encoding<T>::encode(val);

		encode(src, tr);
	}

	template <typename T>
	struct convert_encoding<std::shared_ptr<T>>
	{
		using source = T;
		using ptr = std::shared_ptr<T>;

		static const T& encode(const ptr &p)
		{
			return *p;
		}

		static ptr decode(T &&src)
		{
			return std::make_shared<T>(std::forward<T>(src));
		}
	};

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
}

#endif
