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

	class GTREE_API translate_stream :
		public treeder_stream, public tree_writer_stream
	{
		public:
			translate_stream();
			~translate_stream();

			error translate(
					const std::function<error()> &read,
					const std::function<error()> &write
					);

			void value(const void *data, std::size_t n) override;
			void child_count(std::size_t n) override;
			bool ok() override;

			error next() override;
			std::size_t size() const override;
			const std::uint8_t* data() const override;
			std::size_t child_count() const override;

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
	error translate(const E &src, D *dest)
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
