#ifndef GULACHEK_GTREE_ENCODING_POLYMORPHIC_VARIANT_HPP
#define GULACHEK_GTREE_ENCODING_POLYMORPHIC_VARIANT_HPP

#include "gulachek/gtree/encoding/encoding.hpp"

#include <variant>

namespace gulachek::gtree
{
	template <typename Base>
	struct polymorphic_visitor
	{
		template <typename T>
		const Base* operator()(const T &t) const
		{
			return &t;
		}
				
		template <typename T>
		Base* operator()(T &t)
		{
			return &t;
		}
	};

	template <typename Base, typename ...Ts>
	class polymorphic_variant
	{
		public:
			typedef std::variant<Ts...> variant_type;

			polymorphic_variant() = default;

			template <typename T>
			polymorphic_variant(T &&t) :
				_var{std::move(t)}
			{}

			typedef variant_type gtree_encoding;

			void gtree_encode(gtree_encoding &enc) const
			{ enc = _var; }

			void gtree_decode(const gtree_encoding &var)
			{ _var = var; }

			Base* operator->()
			{
				polymorphic_visitor<Base> visitor;
				return std::visit(visitor, _var);
			}

			const Base* operator->() const
			{
				const polymorphic_visitor<Base> visitor;
				return std::visit(visitor, _var);
			}

			Base& operator*()
			{
				return *operator->();
			}

			const Base& operator*() const
			{
				return *operator->();
			}

		private:
			variant_type _var;
	};
}

#endif
