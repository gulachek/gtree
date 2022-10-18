#include "gulachek/gtree/tree.hpp"

namespace gulachek::gtree
{
	bool tree::empty() const
	{ return value_.empty() && children_.empty(); }

	tree::value_type tree::value() const
	{ return {value_.data(), value_.size()}; }

	void tree::value(const tree::value_type &val)
	{
		value_ = std::vector<std::uint8_t>{val.begin(), val.end()};
	}

	std::size_t tree::child_count() const
	{ return children_.size(); }

	void tree::child_count(std::size_t n)
	{ children_.resize(n); }

	const tree& tree::child(std::size_t i) const
	{ return children_[i]; }

	tree& tree::child(std::size_t i)
	{ return children_[i]; }

	error tree::gtree_decode(treeder &reader)
	{
		value(reader.value());

		auto n = reader.child_count();
		child_count(n);

		for (std::size_t i = 0; i < n; ++i)
		{
			if (auto err = reader.read(&children_[i]))
			{
				return err.wrap() << "error decoding tree child " << i;
			}
		}

		return {};
	}

	error tree::gtree_encode(tree_writer &writer) const
	{
		writer.value(value_.data(), value_.size());

		auto cc = child_count();
		writer.child_count(cc);

		for (std::size_t i = 0; i < cc; ++i)
			writer.write(children_[i]);

		return {};
	}

	// tree_treeder_stream
	tree_treeder_stream::tree_treeder_stream(const tree *tr) :
		pending_{tr}
	{
		base_.child_count(1);
		stack_.push({&base_, 0});
	}

	error tree_treeder_stream::next()
	{
		while (!stack_.empty())
		{
			auto &e = stack_.top();
			auto cc = e.tr->child_count();

			if (pending_)
			{
				stack_.push({pending_, 0});
				pending_ = pending_->empty() ?
					nullptr : &pending_->child(0);
				return {};
			}
			else if (++e.child < cc)
			{
				pending_ = &e.tr->child(e.child);
			}
			else
			{
				stack_.pop();
			}
		}

		return error::eof();
	}

	const tree* tree_treeder_stream::tr() const
	{
		return stack_.top().tr;
	}

	std::size_t tree_treeder_stream::size() const
	{
		return tr()->value().size();
	}

	const std::uint8_t* tree_treeder_stream::data() const
	{
		return tr()->value().data();
	}

	std::size_t tree_treeder_stream::child_count() const
	{
		return tr()->child_count();
	}

	tree_tree_writer_stream::tree_tree_writer_stream(tree *tr)
	{
		base_.child_count(2);
		stack_.push({&base_, 1});
		stack_.push({tr, 0});
	}

	void tree_tree_writer_stream::value(const void *data, std::size_t n)
	{
		tr()->value({(const std::uint8_t*)data, n});
	}

	void tree_tree_writer_stream::child_count(std::size_t n)
	{
		tr()->child_count(n);

		while (!stack_.empty())
		{
			auto &e = stack_.top();
			auto cc = e.tr->child_count();

			if (e.child < cc)
			{
				stack_.push({&e.tr->child(e.child), 0});
				++e.child;
				return;
			}
			else
			{
				stack_.pop();
			}
		}
	}

	bool tree_tree_writer_stream::ok()
	{
		return !stack_.empty();
	}

	tree* tree_tree_writer_stream::tr()
	{
		return stack_.top().tr;
	}

	void tree_tree_writer_stream::next()
	{
	}
}
