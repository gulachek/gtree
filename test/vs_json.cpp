#include <iostream>
#include <cstdlib>
#include <sstream>
#include <fstream>
#include <iterator>
#include <boost/timer/timer.hpp>
#include <boost/json.hpp>
#include <gulachek/gtree.hpp>

namespace gt = gulachek::gtree;

std::string readfile(std::ifstream &stream)
{
	std::istreambuf_iterator<char> it{stream}, end;
	return std::string{it, end};
}

int main(int argc, char **argv)
{
	std::ostringstream usage;
	usage << argv[0] << " <json> <gtree> <n-iterations>";

	if (argc == 1)
	{
		std::cerr << usage.str() << std::endl;
		std::exit(EXIT_SUCCESS);
	}

	if (argc != 4)
	{
		std::cerr << usage.str() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	auto json_path = argv[1];
	auto gtree_path = argv[2];
	std::istringstream niter_ss{argv[3]};
	std::size_t niter = 100;
	niter_ss >> niter;

	std::ifstream json_file{json_path}, gtree_file{gtree_path};
	if (!json_file)
	{
		std::cerr << "Failed to open: " << json_path << std::endl;
		std::exit(EXIT_FAILURE);
	}

	if (!gtree_file)
	{
		std::cerr << "Failed to open: " << gtree_path << std::endl;
		std::exit(EXIT_FAILURE);
	}

	auto json = readfile(json_file);
	auto gtree = readfile(gtree_file);

	std::cout << "Iteration count: " << niter << std::endl;

	std::cout << "JSON" << std::endl;
	{
		boost::system::error_code ec;
		boost::timer::cpu_timer t;
		t.start(); t.stop();

		for (volatile std::size_t i = 0; i < niter; i++)
		{
			std::istringstream ss{json};

			t.resume();
			std::string json_src = ss.str();
			volatile auto jv = boost::json::parse(json_src, ec);
			t.stop();
		}

		std::cout << t.format() << std::endl;
	}

	std::cout << "gtree" << std::endl;
	{
		boost::timer::cpu_timer t;
		t.start(); t.stop();

		for (volatile std::size_t i = 0; i < niter; i++)
		{
			std::istringstream ss{gtree};
			gt::itreem reader{ss};

			t.resume();
			gt::tree tr;
			reader.read(&tr);
			t.stop();
		}

		std::cout << t.format();
	}

	std::exit(EXIT_SUCCESS);
}
