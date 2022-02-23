#ifndef GULACHEK_GTREE_DECODING_HPP
#define GULACHEK_GTREE_DECODING_HPP

#include "gulachek/gtree/my_same_as.hpp"
#include "gulachek/gtree/base128.hpp"
#include "gulachek/gtree/ignore.hpp"

#include <gulachek/cause.hpp>

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

		{dec.decode(r)} -> my_same_as<cause>;
	};

	enum class read_error
	{
		incomplete_value_size = 1,
		incomplete_value,
		incomplete_child_count,
		bad_children
	};

	class treeder
	{
		public:
			treeder(std::istream &is) :
				is_{is},
				nchildren_{1},
				read_count_{0},
				pbuf_{&buf_}
			{}

			std::span<const std::uint8_t> value() const
			{
				return {pbuf_->data(), pbuf_->size()};
			}

			std::size_t child_count() const
			{
				return nchildren_;
			}

			template <decodable Decodable>
			cause read(Decodable *target);
		private:
			std::istream &is_;
			std::size_t nchildren_;
			std::size_t read_count_;
			std::vector<std::uint8_t> buf_;
			std::vector<std::uint8_t> *pbuf_;
	};

	template <typename T>
	concept class_decodable = requires
	(
	 T &val,
	 treeder &reader
	)
	{
		{ val.gtree_decode(reader) } -> my_same_as<cause>;
	};

	template <class_decodable T>
	struct decoding<T>
	{
		T *out;

		cause decode(treeder &r)
		{ return out->gtree_decode(r); }
	};

	template <>
	struct decoding<ignore>
	{
		ignore *ig_;

		cause decode(treeder &r)
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
	cause treeder::read(Decodable *target)
	{
		++read_count_;
		if (read_count_ > nchildren_)
			throw std::logic_error{"Reading too many trees"};

		decoding<Decodable> dec{target};

		std::size_t nbytes;
		if (auto err = read_base128(is_, &nbytes))
		{
			if (err.is_eof())
				return err;

			cause wrap{read_error::incomplete_value_size};
			wrap << "error reading value size";
			wrap.add_cause(err);
			return wrap;
		}

		pbuf_->resize(nbytes);
		is_.read((char*)pbuf_->data(), nbytes);

		auto read_count = is_.gcount();
		if (read_count != nbytes)
		{
			cause err{read_error::incomplete_value};
			err << "reading tree value expected " << nbytes <<
				" bytes, actual: " << read_count;
			return err;
		}

		treeder reader{is_};
		reader.pbuf_ = pbuf_;

		if (auto err = read_base128(is_, &reader.nchildren_))
		{
			cause wrap{read_error::incomplete_child_count};
			wrap << "error reading child count";
			wrap.add_cause(err);
			return wrap;
		}

		if (auto err = dec.decode(reader))
		{
			cause wrap{read_error::bad_children, "handling tree children"};
			wrap.add_cause(err);
			return wrap;
		}

		for (std::size_t i = reader.read_count_;
				i < reader.nchildren_; ++i)
		{
			ignore ig;
			decoding<ignore> ig_dec{&ig};

			if (auto err = dec.decode(reader))
			{
				cause wrap{read_error::bad_children, "handling excess child"};
				wrap.add_cause(err);
				return wrap;
			}
		}

		return {};
	}
}

#endif
