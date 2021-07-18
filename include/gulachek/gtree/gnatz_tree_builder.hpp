#ifndef GULACHEK_GTREE_GNATZ_TREE_BUILDER_HPP
#define GULACHEK_GTREE_GNATZ_TREE_BUILDER_HPP

#include "gulachek/gtree/tree_base.hpp"
#include "gulachek/gtree/tree.hpp"
#include "gulachek/gtree/block.hpp"
#include "gulachek/gtree/gnatz_iter.hpp"

namespace gulachek::gtree
{
	class gnatz_tree_builder
	{
		public:
			block node_block(const block &b)
			{
				return node_block(gnatz_iter{b.data(), b.size()});
			}

			template <typename NumIter>
			block node_block(NumIter &&nums)
			{
				if (!_tb) _tb = std::make_shared<tree_base>();

				std::size_t node_count = 0;
				if (!nums.next(&node_count) || node_count < 1)
					return {};

				_tb->nodes.resize(node_count);

				std::size_t total_size = 0, first_child = 1;
				std::size_t i = 0, size, child_count;
				while (next_node(nums, &size, &child_count))
				{
					if (i >= node_count)
						throw std::runtime_error{"node_block: expected fewer nodes"};

					if (first_child <= i)
						throw std::runtime_error{"node_block: expected more children"};

					node &node = _tb->nodes[i];

					node.value = block{node.value.data(), size};
					total_size += size;

					node.child_count = child_count;
					node.first_child = first_child;
					first_child += child_count;

					i++;
				}

				if (i != node_count)
					throw std::runtime_error{"node_block: expected more nodes"};

				if (first_child != node_count)
					throw std::runtime_error{"node_block: expected fewer children"};

				_tb->data.resize(total_size);

				std::size_t offset = 0;
				for (auto &node : _tb->nodes)
				{
					auto blk = node.value;
					node.value = block{&_tb->data.data()[offset], blk.size()};
					offset += blk.size();
				}

				return {_tb->data.data(), offset};
			}

			tree data_block()
			{
				if (!(_tb && node_count() > 0))
					return {};

				auto tr = tree{_tb, 0};
				_tb.reset();

				return tr;
			}

			std::size_t node_count() const
			{
				return _tb->nodes.size();
			}

			std::size_t node_size(std::size_t i) const
			{
				return _tb->nodes[i].value.size();
			}

			const void* node_address(std::size_t i) const
			{
				return _tb->nodes[i].value.data();
			}

			std::size_t child_count(std::size_t i) const
			{
				return _tb->nodes[i].child_count;
			}

			std::size_t first_child(std::size_t i) const
			{
				return _tb->nodes[i].first_child;
			}

			std::size_t data_size() const
			{
				return _tb->data.size();
			}

			const void* data_address() const
			{
				return _tb->data.data();
			}

		private:
			std::shared_ptr<tree_base> _tb;

			template <typename NumIter>
			bool next_node(
					NumIter &nums,
					std::size_t *size,
					std::size_t *child_count
					)
			{
				if (!nums.next(size))
					return false;

				if (!nums.next(child_count))
					throw std::runtime_error{"node_block: expected child count"};

				return true;
			}
	};
}

#endif
