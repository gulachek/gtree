#include "gulachek/gtree/flat_tree.hpp"
#include "gulachek/gtree/base128.hpp"

namespace gulachek::gtree
{
	std::span<const std::uint8_t> flat_tree::buf() const
	{
		// i promise i'll be a good boy
		auto &ss = const_cast<std::strstream&>(ss_);

		std::span<const std::uint8_t> out{
			(const std::uint8_t*)ss.str(), (std::size_t)ss.pcount()
		};

		ss.freeze(false);
		return out;
	}

	void flat_tree::reset()
	{
		ss_ = std::strstream{};
	}

	cause flat_tree::gtree_encode(tree_writer &w) const
	{
		auto b = buf();

		// nothing written yet - assume empty tree
		if (b.empty())
		{
			w.value(nullptr, 0);
			w.child_count(0);
			return {};
		}

		return w.write_raw(b.data(), b.size());
	}

	cause flat_tree::gtree_decode(treeder &r)
	{
		bool is_top_level = !is_recursing_;

		if (is_top_level)
			reset();

		auto val = r.value();
		if (auto err = write_base128(ss_, val.size()))
		{
			cause wrap{"error writing value size"};
			wrap.add_cause(err);
			return wrap;
		}

		if (!ss_.write((const char*)val.data(), val.size()))
		{
			return {"error writing value data"};
		}

		auto cc = r.child_count();
		if (auto err = write_base128(ss_, cc))
		{
			cause wrap{"error writing child count"};
			wrap.add_cause(err);
			return wrap;
		}

		for (std::size_t i = 0; i < cc; ++i)
		{
			is_recursing_ = true;
			if (auto err = r.read(this))
			{
				cause wrap;
				wrap << "error reading child " << i;
				wrap.add_cause(err);
				return wrap;
			}
		}

		if (is_top_level)
		{
			is_recursing_ = false;
			ss_.seekg(0, std::ios::beg);
		}
		return {};
	}
}
