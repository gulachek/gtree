#ifndef GULACHEK_GTREE_TREED_HPP
#define GULACHEK_GTREE_TREED_HPP

#include <istream>
#include <type_traits>

#include <vector>
#include <sstream>

namespace gulachek::gtree
{
	enum class standard_code
	{
		generic,
		eof,
		stub // (not implemented)
	};

	class cause
	{
		public:
			cause(std::string desc = "") :
				code_{standard_code::generic},
				ss_{desc}
			{}

			static cause eof()
			{
				cause out;
				out.code_ = standard_code::eof;
				return out;
			}

			template <typename T>
			cause& format(const T &rhs)
			{
				ss_ << rhs;
				return *this;
			}

			bool is_eof() const
			{ return code_ == standard_code::eof; }

			template <typename T>
			cause& operator << (const T &rhs)
			{ return format<T>(rhs); }

			template <typename Cause>
			void add_cause(Cause &&c)
			{ causes_.emplace_back(std::forward<Cause>(c)); }

		private:
			standard_code code_;
			std::stringstream ss_;
			std::vector<cause> causes_;
	};

	template <typename T, typename U>
	concept my_same_as = std::is_same_v<T, U>;

	template <typename T>
	struct decoding {};

	template <typename T>
	concept decodable = requires (T *p, std::istream &is)
	{
		requires std::is_default_constructible_v<T>;
		{typename decoding<T>::decode(is, p)} -> my_same_as<cause>;
	};

	struct decoding<mutable_tree>
	{
		mutable_tree *out;

		cause value(std::size_t n, void *data)
		{
			out->value({n, data});
			return {};
		}

		cause children(std::size_t n, child_reader &r)
		{
			out->child_count(n);
			for (std::size_t i = 0; i < n; ++i)
			{
				if (auto err = r.read(&out->child(i)))
				{
					cause wrap;
					wrap << "error decoding mutable_tree child " << i;
					wrap.add_cause(err);
					return wrap;
				}
			}

			return {};
		}
	};

	class child_reader
	{
		public:
			child_reader(std::istream &is) :
				is_{is}
			{}

			template <decodable Decodable>
			cause read(Decodable *target)
			{
				decoding<Decodable> dec{target};

				std::size_t nbytes;
				if (auto err = read_num(is, &nbytes))
				{
					if (err.is_eof())
						return err;

					cause wrap;
					wrap << "error reading value size";
					wrap.add_cause(err);
					return wrap;
				}

				buf_.resize(nbytes);
				is.read((char*)buf_.data(), nbytes);

				auto read_count = is.gcount();
				if (read_count != nbytes)
				{
					cause err;
					err << "reading tree value expected " << nbytes <<
						" bytes, actual: " << read_count;
					return err;
				}

				// invoke encoding specific logic
				if (auto err = dec.value(nbytes, buf_.data()))
				{
					cause wrap{"handling tree value"};
					wrap.add_cause(err);
					return wrap;
				}

				std::size_t nchildren;
				if (auto err = read_num(is, &nchildren))
				{
					cause wrap;
					wrap << "error reading child count";
					wrap.add_cause(err);
					return wrap;
				}

				// handle each child
				if (auto err = dec.children(nchildren, this))
				{
					cause wrap{"handling tree children"};
					wrap.add_cause(err);
					return wrap;
				}

				return {};
			}

		private:
			std::istream &is_;
			std::vector<std::uint8_t> buf_;
	};

	template <decodable Decodable>
	cause treed(std::istream &is, Decodable *target)
	{
		child_reader reader{is};
		return reader.read(target);
	}

}

#endif
