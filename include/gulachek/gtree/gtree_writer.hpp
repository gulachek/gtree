#ifndef GULACHEK_GTREE_GTREE_WRITER_HPP
#define GULACHEK_GTREE_GTREE_WRITER_HPP

#include "gulachek/gtree/block.hpp"

namespace gulachek::gtree
{
	template <
		typename BFS,
		typename NumberWriter,
		typename OutStream
			>
	class gtree_writer
	{
		public:
			gtree_writer(
					BFS &bfs,
					NumberWriter &nums,
					OutStream &os
					) :
				_bfs{bfs}, _nums{nums}, _os{os}
			{}

			template <typename Tree>
			void write(const Tree &t)
			{
				std::size_t node_block_size = 0;
				std::size_t node_count = 0;

				// Write node block size then node count
				_bfs.visit(t,
				[&](const block &b, std::size_t nchild){
					node_block_size +=
						_nums.plan(b.size()) + _nums.plan(nchild);

					++node_count;
				});

				node_block_size += _nums.plan(node_count);

				_nums.write(node_block_size);
				_nums.write(node_count);

				// Write each nodes size & child count
				_bfs.visit(t,
				[&](const block &b, std::size_t nchild){
					_nums.write(b.size());
					_nums.write(nchild);
				});

				// Write each value
				_bfs.visit(t,
				[&](const block &b, std::size_t nchild){
					_os.write((const char*)b.data(), b.size());
				});
			}

		private:
			BFS &_bfs;
			NumberWriter &_nums;
			OutStream &_os;
	};
}

#endif
