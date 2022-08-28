#ifndef GULACHEK_GTREE_TREE_HPP
#define GULACHEK_GTREE_TREE_HPP

#include "gulachek/gtree/decoding.hpp"
#include "gulachek/gtree/encoding.hpp"

#include <span>
#include <cstddef>
#include <cstdint>
#include <utility>
#include <vector>
#include <stack>

namespace gulachek::gtree
{
	template <typename T, typename Elem>
	concept vector_like = requires (T &&val)
	{
		std::vector<Elem>{std::forward<T>(val)};
	};

	class tree
	{
		public:
			using value_type = std::span<const std::uint8_t>;

			GTREE_API tree() = default;

			template <
				vector_like<std::uint8_t> Value,
				vector_like<tree> Children
					>
			GTREE_API tree(
					Value &&value,
					Children &&children
					) :
				value_{std::forward<Value>(value)},
				children_{std::forward<Children>(children)}
			{}

			template < vector_like<std::uint8_t> Value >
			GTREE_API tree(
					Value &&value
					) :
				value_{std::forward<Value>(value)},
				children_{}
			{}

			GTREE_API bool empty() const;

			GTREE_API value_type value() const;
			GTREE_API void value(const value_type &val);

			GTREE_API std::size_t child_count() const;
			GTREE_API void child_count(std::size_t n);

			GTREE_API const tree& child(std::size_t i) const;
			GTREE_API tree& child(std::size_t i);

			GTREE_API cause gtree_encode(tree_writer &writer) const;
			GTREE_API cause gtree_decode(treeder &reader);

			// optimized translation from tree
			template <decodable D>
			GTREE_API cause read(D *dest) const;

			// optimized translation to tree
			template <encodable E>
			GTREE_API cause write(const E &src);

		private:
			std::vector<std::uint8_t> value_;
			std::vector<tree> children_;
	};

	class GTREE_API tree_treeder_stream : public treeder_stream
	{
		public:
			tree_treeder_stream(const tree *tr);
			~tree_treeder_stream() = default;

			cause next() override;
			std::size_t size() const override;
			const std::uint8_t* data() const override;
			std::size_t child_count() const override;

		private:
			struct elem
			{
				const tree *tr;
				std::size_t child;
			};

			tree base_;
			const tree* pending_;
			std::stack<elem> stack_;

			const tree* tr() const;
	};

	template <decodable D>
	cause tree::read(D *dest) const
	{
		tree_treeder_stream stream{this};
		treeder reader{stream};
		return reader.read(dest);
	}

	class GTREE_API tree_tree_writer_stream : public tree_writer_stream
	{
		public:
			tree_tree_writer_stream(tree *tr);

			void value(const void *data, std::size_t n) override;
			void child_count(std::size_t n) override;
			bool ok() override;

		private:
			struct elem
			{
				tree *tr;
				std::size_t child;
			};

			tree base_;
			std::stack<elem> stack_;

			tree* tr();
			void next();
	};

	template <encodable E>
	cause tree::write(const E &src)
	{
		tree_tree_writer_stream stream{this};
		tree_writer writer{stream};
		return writer.write(src);
	}
}

#endif
