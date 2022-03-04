#ifndef GULACHEK_GTREE_FLAT_TREE_HPP
#define GULACHEK_GTREE_FLAT_TREE_HPP

#include "gulachek/gtree/write.hpp"
#include "gulachek/gtree/read.hpp"

#include <strstream>
#include <span>
#include <cstdint>
#include <functional>

namespace gulachek::gtree
{
	class flat_tree
	{
		public:
			flat_tree() = default;

			std::span<const std::uint8_t> buf() const;

			template <encodable T>
			cause write(const T &val)
			{
				reset();
				auto err = gtree::write(ss_, val);
				ss_.seekg(0, std::ios::beg);
				return err;
			}

			template <decodable T>
			cause read(T *val) const
			{
				auto &ss = const_cast<std::strstream&>(ss_);

				auto err = gtree::read(ss, val);
				ss.seekg(0, std::ios::beg);
				return err;
			}

			cause gtree_encode(tree_writer &w) const;
			cause gtree_decode(treeder &r);

		private:
			void reset();

			std::strstream ss_;
			bool is_recursing_ = false;
	};
}

#endif
