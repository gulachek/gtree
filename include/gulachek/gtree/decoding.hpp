#ifndef GULACHEK_GTREE_DECODING_HPP
#define GULACHEK_GTREE_DECODING_HPP

#include "gulachek/gtree/my_same_as.hpp"
#include "gulachek/gtree/base128.hpp"
#include "gulachek/gtree/ignore.hpp"

#include <gulachek/error.hpp>

#include <span>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <stdexcept>

namespace gulachek::gtree
{
	template <typename T>
	struct decoding {};

	class treeder;

	template <typename T>
	concept decodable = requires
		(
		 T *p,
		 decoding<T> dec,
		 treeder &r
		 )
	{
		typename decoding<T>;
		{ decoding<T>{p} };

		{dec.decode(r)} -> my_same_as<error>;
	};

	enum class read_error
	{
		incomplete_value_size = 1,
		incomplete_value,
		incomplete_child_count,
		bad_children
	};

	struct GTREE_API treeder_stream
	{
		virtual ~treeder_stream(){}
		virtual error next() = 0;

		virtual std::size_t size() const = 0;
		virtual const std::uint8_t* data() const = 0;
		virtual std::size_t child_count() const = 0;
	};

	class GTREE_API istream_treeder_stream : public treeder_stream
	{
		public:
			istream_treeder_stream(std::istream &is) :
				is_{is}
			{}

			error next() override
			{
				std::size_t nbytes;
				if (auto err = read_base128(is_, &nbytes))
				{
					if (err.is_eof())
						return err;

					error wrap{read_error::incomplete_value_size};
					wrap << "error reading value size";
					wrap.add_cause(err);
					return wrap;
				}

				buf_.resize(nbytes);
				is_.read((char*)buf_.data(), nbytes);

				auto read_count = (std::size_t)is_.gcount();
				if (read_count != nbytes)
				{
					error err{read_error::incomplete_value};
					err << "reading tree value expected " << nbytes <<
						" bytes, actual: " << read_count;
					return err;
				}

				if (auto err = read_base128(is_, &child_count_))
				{
					error wrap{read_error::incomplete_child_count};
					wrap << "error reading child count";
					wrap.add_cause(err);
					return wrap;
				}

				return {};
			}

			std::size_t size() const override
			{ return buf_.size(); }

			const std::uint8_t* data() const override
			{ return buf_.data(); }

			std::size_t child_count() const override
			{ return child_count_; }

		private:
			std::istream &is_;
			std::vector<std::uint8_t> buf_;
			std::size_t child_count_;
	};

	class GTREE_API treeder
	{
		public:
			treeder(treeder_stream &s) :
				stream_{s},
				nchildren_{1},
				read_count_{0}
			{}

			std::span<const std::uint8_t> value() const
			{
				return {stream_.data(), stream_.size()};
			}

			std::size_t child_count() const
			{
				return nchildren_;
			}

			template <decodable Decodable>
			error read(Decodable *target);

		private:
			treeder_stream &stream_;
			std::size_t nchildren_;
			std::size_t read_count_;
	};

	template <typename T>
	concept class_decodable = requires
	(
	 T &val,
	 treeder &reader
	)
	{
		{ val.gtree_decode(reader) } -> my_same_as<error>;
	};

	template <class_decodable T>
	struct decoding<T>
	{
		T *out;

		error decode(treeder &r)
		{ return out->gtree_decode(r); }
	};

	template <>
	struct GTREE_API decoding<ignore>
	{
		ignore *ig_;

		error decode(treeder &r)
		{
			auto cc = r.child_count();
			for (std::size_t i = 0; i < cc; ++i)
			{
				if (auto err = r.read(ig_))
					return err;
			}

			return {};
		}
	};

	template <decodable Decodable>
	error treeder::read(Decodable *target)
	{
		++read_count_;
		if (read_count_ > nchildren_)
			throw std::logic_error{"Reading too many trees"};

		if (auto err = stream_.next())
			return err;

		treeder reader{stream_};
		reader.nchildren_ = stream_.child_count();

		decoding<Decodable> dec{target};

		if (auto err = dec.decode(reader))
		{
			error wrap{read_error::bad_children, "decoding tree"};
			wrap.add_cause(std::move(err));
			return wrap;
		}

		for (std::size_t i = reader.read_count_;
				i < reader.nchildren_; ++i)
		{
			ignore ig;
			treeder ig_reader{stream_};

			if (auto err = ig_reader.read(&ig))
			{
				error wrap{read_error::bad_children, "handling excess child"};
				wrap.add_cause(std::move(err));
				return wrap;
			}
		}

		return {};
	}
}

#endif
