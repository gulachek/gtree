#ifndef GULACHEK_GTREE_TREE_HPP
#define GULACHEK_GTREE_TREE_HPP

#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <span>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>

namespace gulachek::gtree
{
	class tree
	{
		public:
			using value_type = std::span<const std::uint8_t>;

			tree() = default;

			template <typename Value, typename Children>
			tree(
					Value &&value,
					Children &&children
					) :
				value_{std::forward<Value>(value)},
				children_{std::forward<Children>(children)}
			{}

			value_type value() const
			{ return {value_.data(), value_.size()}; }

			void value(const value_type &val)
			{
				value_ = std::vector<std::uint8_t>{val.begin(), val.end()};
			}

			std::size_t child_count() const
			{ return children_.size(); }

			void child_count(std::size_t n)
			{ children_.resize(n); }

			const tree& child(std::size_t i) const
			{ return children_[i]; }

			tree& child(std::size_t i)
			{ return children_[i]; }

			cause gtree_encode(tree_writer &writer) const
			{
				writer.value(value_.data(), value_.size());

				auto cc = child_count();
				writer.child_count(cc);

				for (std::size_t i = 0; i < cc; ++i)
					writer.write(children_[i]);

				return {};
			}

			cause gtree_decode(treeder &reader)
			{
				value(reader.value());

				auto n = reader.child_count();
				child_count(n);

				for (std::size_t i = 0; i < n; ++i)
				{
					if (auto err = reader.read(&children_[i]))
					{
						cause wrap;
						wrap << "error decoding tree child " << i;
						wrap.add_cause(err);
						return wrap;
					}
				}

				return {};
			}

		private:
			std::vector<std::uint8_t> value_;
			std::vector<tree> children_;
	};
}

#endif
