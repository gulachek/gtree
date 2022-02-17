#ifndef GULACHEK_CAUSE_HPP
#define GULACHEK_CAUSE_HPP

#include <cstddef>
#include <vector>
#include <sstream>

namespace gulachek
{
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

			template <typename T> requires
				std::is_constructible_v<std::size_t, T>
			cause(T ucode, const char* desc = "") :
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

			std::size_t ucode() const
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
			std::size_t ucode_;
			std::stringstream ss_;
			std::vector<cause> causes_;
	};

}

#endif
