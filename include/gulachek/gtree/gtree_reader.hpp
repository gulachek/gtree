#ifndef GULACHEK_GTREE_GTREE_READER_HPP
#define GULACHEK_GTREE_GTREE_READER_HPP

#include "gulachek/gtree/tree.hpp"

namespace gulachek::gtree
{
	template<
		typename NatNumReader,
		typename InStream,
		typename TreeBuilder
			>
	class gtree_reader
	{
		public:
			gtree_reader(
					NatNumReader &nums,
					InStream &is,
					TreeBuilder &builder
					) :
				_nums{nums}, _is{is}, _builder{builder}
			{}

			bool read(tree *ptree)
			{
				std::size_t node_block_size;
				if (!_nums.read(&node_block_size))
				{
					return false; // eof
				}

				std::vector<std::uint8_t> buf;
				buf.resize(node_block_size);
				if (!_is.read((char*) buf.data(), buf.size()))
				{
					throw std::runtime_error{"Not enough data in node block"};
				}

				const block node_block{buf.data(), buf.size()};

				auto data_block = _builder.node_block(node_block);
				if (!_is.read(
							(char*) data_block.data(), data_block.size()))
				{
					throw std::runtime_error{"Not enough data in data block"};
				}

				*ptree = _builder.data_block();

				return true;
			}

		private:
			NatNumReader &_nums;
			InStream &_is;
			TreeBuilder &_builder;
	};
}

#endif
