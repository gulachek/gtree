#ifndef GULACHEK_GTREE_OTREEM_HPP
#define GULACHEK_GTREE_OTREEM_HPP

#include "gulachek/gtree/is_otreem.hpp"
#include "gulachek/gtree/gtree_writer.hpp"
#include "gulachek/gtree/bfs.hpp"
#include "gulachek/gtree/gnatz_writer.hpp"

#include <ostream>

namespace gulachek::gtree
{
	template <typename Stream>
	class basic_otreem
	{
		public:
			basic_otreem(Stream &stream) :
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

	typedef basic_otreem<std::ostream> otreem;

	template <typename Ostream>
	struct is_otreem<basic_otreem<Ostream>> :
		std::true_type {};
}

#endif
