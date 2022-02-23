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

		private:
			std::vector<std::uint8_t> value_;
			std::vector<tree> children_;
	};

	template <>
	struct decoding<tree>
	{
		tree *out;

		cause value(std::size_t n, std::uint8_t *data)
		{
			out->value({data, data + n});
			return {};
		}

		cause children(std::size_t n, treeder &r)
		{
			out->child_count(n);
			for (std::size_t i = 0; i < n; ++i)
			{
				if (auto err = r.read(&out->child(i)))
				{
					cause wrap;
					wrap << "error decoding tree child " << i;
					wrap.add_cause(err);
					return wrap;
				}
			}

			return {};
		}
	};

	template <>
	struct encoding<tree>
	{
		const tree &tr_;

		cause encode(tree_writer &writer)
		{
			auto val = tr_.value();
			writer.value(val.data(), val.size());

			auto cc = tr_.child_count();
			writer.child_count(cc);

			for (std::size_t i = 0; i < cc; ++i)
				writer.write(tr_.child(i));

			return {};
		}
	};
}

#endif
