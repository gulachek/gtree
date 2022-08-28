#ifndef GULACHEK_GTREE_TRANSLATE_HPP
#define GULACHEK_GTREE_TRANSLATE_HPP

#include "gulachek/gtree/encoding.hpp"
#include "gulachek/gtree/decoding.hpp"

#include <memory>
#include <functional>

namespace gulachek::gtree
{
	enum class translate_error
	{
		encoding,
		decoding
	};

	class translate_stream :
		public treeder_stream, public tree_writer_stream
	{
		public:
			GTREE_API translate_stream();
			GTREE_API ~translate_stream();

			GTREE_API cause translate(
					const std::function<cause()> &read,
					const std::function<cause()> &write
					);

			GTREE_API void value(const void *data, std::size_t n) override;
			GTREE_API void child_count(std::size_t n) override;
			GTREE_API bool ok() override;

			GTREE_API cause next() override;
			GTREE_API std::size_t size() const override;
			GTREE_API const std::uint8_t* data() const override;
			GTREE_API std::size_t child_count() const override;

		private:
			// don't want to force consumer to include boost::fiber
			struct fiber;
			std::unique_ptr<fiber> pfiber_;

			std::vector<std::uint8_t> buf_;
			std::size_t child_count_;

			bool is_reading_ = true;
			bool ok_ = true;

			void done_reading(bool ok);
			void done_writing();
	};

	template <encodable E, decodable D>
	cause translate(const E &src, D *dest)
	{
		translate_stream translator;

		treeder r{translator};
		tree_writer w{translator};

		return translator.translate(
			[&]{ return r.read(dest); },
			[&]{ return w.write(src); }
			);
	}
}

#endif
