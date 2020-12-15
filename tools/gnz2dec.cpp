#include "gulachek/gtree/gnatz_reader.hpp"
#include "gulachek/gtree/gnatz_writer.hpp"

#include <iostream>
#include <fstream>
#include <string>

namespace gt = gulachek::gtree;

template <typename InStream, typename OutStream>
void gnz2dec(InStream &&in, OutStream &&out)
{
	gt::gnatz_reader gnz{in};
	std::size_t n;
	while (gnz.read(&n))
		out << n << std::endl;
}

template <typename InStream, typename OutStream>
void dec2gnz(InStream &&in, OutStream &&out)
{
	gt::gnatz_writer gnz{out};
	std::size_t n;
	while (in >> n)
		gnz.write(n);
}

int main(int argc, char** argv)
{
	try
	{
		if (argc < 2)
		{
			gnz2dec(std::cin, std::cout);
		}
		else if (argc == 2 && std::string{argv[1]} == "-r")
		{
			dec2gnz(std::cin, std::cout);
		}
		else
		{
			std::cerr << "Usage: " << argv[0] << " [-r]" << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << std::endl;
		return EXIT_FAILURE;
	}
}
