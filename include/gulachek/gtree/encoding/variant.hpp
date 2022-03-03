#ifndef GULACHEK_GTREE_ENCODING_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_VARIANT_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/encoding/unsigned.hpp"

#include <variant>
#include <type_traits>

namespace gulachek::gtree
{
	template <typename ...Ts>
	struct decoding<std::variant<Ts...>>
	{
		static constexpr std::size_t n = sizeof...(Ts);
		using var_t = std::variant<Ts...>;

		var_t *pv;

		cause decode(treeder &r)
		{
			std::size_t actual_index;
			if (auto err = decode_unsigned(r.value(), &actual_index))
			{
				cause wrap{"failed to decode variant alt index"};
				wrap.add_cause(err);
				return wrap;
			}

			if (!r.child_count())
			{
				return {"tree must have child to decode variant"};
			}

			return decode_alt<0>(actual_index, r);
		}

		template <std::size_t index>
			requires (index == n)
		cause decode_alt(std::size_t actual_index, treeder &)
		{
			cause err;
			err << "variant alt index " << actual_index << " is out of "
				"bounds for a variant with " << n << " alts";
			return err;
		}

		template <std::size_t index>
			requires (index < n)
		cause decode_alt(std::size_t actual_index, treeder &r)
		{
			if (actual_index == index)
			{
				using alt_t = std::variant_alternative_t<index, var_t>;
				alt_t val;
				
				if (auto err = r.read(&val))
				{
					cause wrap;
					wrap << "error reading variant alt " << index;
					wrap.add_cause(err);
					return wrap;
				}

				pv->template emplace<index>(std::move(val));
				return {};
			}

			return decode_alt<index+1>(actual_index, r);
		}
	};

	template <typename ...Ts>
	struct encoding<std::variant<Ts...>>
	{
		static constexpr size_t n = sizeof...(Ts);
		using var_t = std::variant<Ts...>;

		const var_t &v;

		cause encode(tree_writer &w)
		{
			std::uint8_t index[sizeof(std::size_t)];
			auto n = encode_unsigned(index, v.index());
			w.value(index, n);

			w.child_count(1);
			return encode_alt<0>(w);
		}

		template <std::size_t index>
			requires (index == n)
		cause encode_alt(tree_writer &)
		{
			throw std::logic_error{"should not happen"};
		}

		template <std::size_t index>
			requires (index < n)
		cause encode_alt(tree_writer &w)
		{
			if (v.index() == index)
			{
				if (auto err = w.write(std::get<index>(v)))
				{
					cause wrap;
					wrap << "error encoding variant alt " << index;
					wrap.add_cause(err);
					return wrap;
				}
				return {};
			}

			return encode_alt<index+1>(w);
		}
	};
}

#endif
