#ifndef GULACHEK_GTREE_OTREEM_HPP
#define GULACHEK_GTREE_OTREEM_HPP

#include "gulachek/gtree/gtree_writer.hpp"
#include "gulachek/gtree/bfs.hpp"
#include "gulachek/gtree/gnatz_writer.hpp"
#include "gulachek/gtree/mutable_tree.hpp"

#include <ostream>

namespace gulachek::gtree
{
	template <typename Stream>
	class otreem
	{
		public:
			otreem(Stream &stream) :
				_stream{stream},
				_nums{stream},
				_bfs{},
				_writer{_bfs, _nums, _stream}
			{}

			template <typename Tree>
			void write(const Tree &t)
			{
				_writer.write(t);
			}

		private:
			typedef gnatz_writer<Stream> NumWriter;
			typedef gtree_writer<bfs, NumWriter, Stream> 
				GtWriter;

			Stream &_stream;
			NumWriter _nums;
			bfs _bfs;
			GtWriter _writer;
	};
}

template <typename OutStream, typename T>
gulachek::gtree::otreem<OutStream>& operator << (
		gulachek::gtree::otreem<OutStream>& ot,
		const T &val
		)
{
	gulachek::gtree::mutable_tree tree;
	gulachek::gtree::encode(val, tree);
	ot.write(tree);
	return ot;
}

#endif
