#ifndef GULACHEK_GTREE_ERROR_HPP
#define GULACHEK_GTREE_ERROR_HPP

#include <string>
#include <ostream>
#include <sstream>

namespace gulachek::gtree
{
	class error
	{
		public:
			error(const std::string &msg = "") :
				msg_{msg}
			{}

			error(const char *msg) :
				error{std::string{msg}}
			{}

			operator bool() const;
			void format(std::ostream &os) const;

			template <typename T>
			error& operator <<(const T &val)
			{
				msg_ << val;
				return *this;
			}

		private:
			std::ostringstream msg_;
	};

	std::ostream& operator <<(std::ostream &os, const error &err);
}

#endif
