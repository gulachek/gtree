#ifndef GULACHEK_GTREE_READ_HPP
#define GULACHEK_GTREE_READ_HPP

#include <istream>
#include <type_traits>

#include <vector>
#include <sstream>
#include <string>
#include <span>
#include <cstdint>
#include <ostream>

namespace gulachek::gtree
{
	template <typename T>
	concept user_defined_codeable =
		std::is_integral_v<T> || std::is_enum_v<T>;

	class user_defined_code
	{
		public:
			template <user_defined_codeable T = std::size_t>
			user_defined_code(T c = T{}) :
				c_{(std::size_t)c}
			{}

			template <user_defined_codeable T>
			bool operator == (const T &rhs) const
			{ return c_ == (std::size_t)rhs; }

			operator bool () const
			{ return c_; }

			std::size_t value() const
			{ return c_; }

		private:
			std::size_t c_;
	};

	class cause
	{
		enum class standard_code
		{
			generic,
			eof,
			stub // (not implemented)
		};

		public:
			cause(std::string desc = "") :
				code_{standard_code::generic},
				ucode_{},
				ss_{desc}
			{}

			template <user_defined_codeable T>
			cause(T ucode, std::string desc = "") :
				code_{standard_code::generic},
				ucode_{ucode},
				ss_{desc}
			{}

			cause(const cause &other) :
				code_{other.code_},
				ucode_{other.ucode_},
				ss_{other.ss_.str()},
				causes_{other.causes_}
			{}

			operator bool () const
			{
				return (code_ != standard_code::generic) ||
					ucode_ ||
					ss_.str().size();
			}

			static cause eof()
			{
				cause out;
				out.code_ = standard_code::eof;
				return out;
			}

			user_defined_code ucode() const
			{ return ucode_; }

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
			user_defined_code ucode_;
			std::stringstream ss_;
			std::vector<cause> causes_;
	};

	template <typename T, typename U>
	concept my_same_as = std::is_same_v<T, U>;

	template <typename T>
	struct decoding {};

	enum class read_error
	{
		incomplete_value_size = 1,
		incomplete_value,
		incomplete_child_count,
		bad_children
	};

	cause read_num(std::istream &is, std::size_t *n)
	{
		*n = 0;
		std::size_t power = 1;
		std::size_t b;

		auto c = is.get();
		auto eof = std::istream::traits_type::eof();

		if (c == eof)
		{
			if (is.eof())
				return cause::eof();
			else
				return {"bad stream"};
		}

		b = (std::size_t) c;
		while (b & 0x80)
		{
			*n += (power * (b & 0x7f));
			power *= 128;

			c = is.get();
			if (c == eof)
				return {"incomplete base 127 integer"};

			b = (std::size_t) c;
		}

		*n += (power * b);
		return {};
	}


	class child_reader
	{
		public:
			child_reader(std::istream &is) :
				is_{is}
			{}

			template <typename Decodable>
			cause read(Decodable *target)
			{
				decoding<Decodable> dec{target};

				std::size_t nbytes;
				if (auto err = read_num(is_, &nbytes))
				{
					if (err.is_eof())
						return err;

					cause wrap{read_error::incomplete_value_size};
					wrap << "error reading value size";
					wrap.add_cause(err);
					return wrap;
				}

				buf_.resize(nbytes);
				is_.read((char*)buf_.data(), nbytes);

				auto read_count = is_.gcount();
				if (read_count != nbytes)
				{
					cause err{read_error::incomplete_value};
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
				if (auto err = read_num(is_, &nchildren))
				{
					cause wrap{read_error::incomplete_child_count};
					wrap << "error reading child count";
					wrap.add_cause(err);
					return wrap;
				}

				// handle each child
				if (auto err = dec.children(nchildren, *this))
				{
					cause wrap{read_error::bad_children, "handling tree children"};
					wrap.add_cause(err);
					return wrap;
				}

				return {};
			}

		private:
			std::istream &is_;
			std::vector<std::uint8_t> buf_;
	};


	template <typename T>
	concept decodable = requires
		(
		 T *p,
		 decoding<T> dec,
		 std::size_t n,
		 std::uint8_t *data,
		 child_reader r
		 )
	{
		requires std::is_default_constructible_v<T>;
		requires std::is_constructible_v<decoding<T>, T*>;

		{dec.value(n, data)} -> my_same_as<cause>;
		{dec.children(n, r)} -> my_same_as<cause>;
	};

	class tree
	{
		public:
			using value_type = std::span<const std::uint8_t>;

			tree() = default;

			template <typename Value, typename Children>
			tree(
					Value &&value,
					Children &&children
					) :
				value_{std::forward<Value>(value)},
				children_{std::forward<Children>(children)}
			{}

			value_type value() const
			{ return {value_.data(), value_.size()}; }

			void value(const value_type &val)
			{
				value_ = std::vector<std::uint8_t>{val.begin(), val.end()};
			}

			std::size_t child_count() const
			{ return children_.size(); }

			void child_count(std::size_t n)
			{ children_.resize(n); }

			const tree& child(std::size_t i) const
			{ return children_[i]; }

			tree& child(std::size_t i)
			{ return children_[i]; }

		private:
			std::vector<std::uint8_t> value_;
			std::vector<tree> children_;
	};

	template <>
	struct decoding<tree>
	{
		tree *out;

		decoding(tree *o) : out{o} {}

		cause value(std::size_t n, std::uint8_t *data)
		{
			out->value({data, data + n});
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
					wrap << "error decoding tree child " << i;
					wrap.add_cause(err);
					return wrap;
				}
			}

			return {};
		}
	};

	template <decodable Decodable>
	cause read(std::istream &is, Decodable *target)
	{
		child_reader reader{is};
		return reader.read(target);
	}

	/*
	template <typename TreeReader, typename T>
		requires Tree<typename TreeReader::tree_type> // make a TreeReader concept later
	error read(TreeReader &reader, T &t)
	{
		typename TreeReader::tree_type tr;

		if (!reader.read(&tr))
			return {"Failed to read tree"};

		if (auto err = decode(tr, t))
			return err;

		return {};
	}

	template <typename T>
	error read(std::istream &is, T &t)
	{
		itreem treem{is};
		return read<itreem, T>(treem, t);
	}

	template <typename T>
	error read_file(const std::filesystem::path &path, T &t)
	{
		std::fstream f{path};
		if (!f)
		{
			error err;
			err << "Failed to open file: " << path;
			return err;
		}

		return read(f, t);
	}
	*/
}

#endif
