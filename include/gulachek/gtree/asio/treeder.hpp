#ifndef GULACHEK_GTREE_ASIO_TREEDER_HPP
#define GULACHEK_GTREE_ASIO_TREEDER_HPP

#include "gulachek/gtree/asio/gtree_reader.hpp"
#include "gulachek/gtree/gnatz_tree_builder.hpp"
#include "gulachek/gtree/asio/gnatz_reader.hpp"
#include "gulachek/gtree/tree.hpp"

namespace gulachek::gtree::asio
{
	// Adaptor for GtreeReader to meet AsyncTreeReader requirements
	template <typename AsyncReadStream>
	class treeder
	{
			typedef gnatz_reader<AsyncReadStream> gnz_type;
			typedef gtree_reader<gnz_type, AsyncReadStream, gnatz_tree_builder> gtree_type;

		public:
			typedef tree tree_type;

			typedef typename gtree_type::executor_type executor_type;

			treeder(AsyncReadStream &stream) :
				_sbp{new boost::asio::streambuf()},
				_gnz{stream, _sbp.get()},
				_builder{},
				_gtree{_gnz, stream, _builder, *_sbp}
			{}

			template <typename CompletionToken>
			auto async_read(CompletionToken &&token)
			{
				return _gtree.async_read(std::forward<CompletionToken>(token));
			}

			executor_type get_executor()
			{
				return _gtree.get_executor();
			}

		private:
			std::unique_ptr<boost::asio::streambuf> _sbp;
			gnz_type _gnz;
			gnatz_tree_builder _builder;

			gtree_reader<
				gnz_type,
				AsyncReadStream,
				gnatz_tree_builder
					> _gtree;
	};
}

#endif
