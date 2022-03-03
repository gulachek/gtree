#ifndef GULACHEK_CAUSE_HPP
#define GULACHEK_CAUSE_HPP

#include <cstddef>
#include <vector>
#include <sstream>
#include <type_traits>

namespace gulachek
{
	template <typename T>
	concept user_defined_codeable = requires (T val)
	{
		{static_cast<std::size_t>(val)};
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
			cause(const char* desc = "") :
				code_{standard_code::generic},
				ucode_{},
				ss_{desc}
			{}

			template <user_defined_codeable T>
			cause(T ucode, const char* desc = "") :
				code_{standard_code::generic},
				ucode_{static_cast<std::size_t>(ucode)},
				ss_{desc}
			{}

			cause(const cause &other) :
				code_{other.code_},
				ucode_{other.ucode_},
				ss_{other.ss_.str()},
				causes_{other.causes_}
			{}

			cause(cause &&other) = default;
			cause& operator =(cause &&other) = default;

			cause& operator = (const cause &other)
			{
				cause temp{other};
				std::swap(*this, temp);
				return *this;
			}

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

			template <user_defined_codeable T>
			bool has_ucode(T c) const
			{ return ucode_ == static_cast<std::size_t>(c); }

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
			std::size_t ucode_;
			std::stringstream ss_;
			std::vector<cause> causes_;
	};

	template <typename T>
	concept __is_cause_ref = std::is_same_v<T, cause&>;

	template <typename T>
	concept cause_writable = requires (T val, cause c)
	{
		{c << val} -> __is_cause_ref;
	};
}

#endif
