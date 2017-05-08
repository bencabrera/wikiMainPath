// stdlib
#include <iostream>
#include <iomanip>
#include <map>
#include <future>
#include <thread>

// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

// local files
#include "helpers/shared.h"
#include "wikiDataCache.h"

// shared library
#include "../../libs/shared/cpp/stepTimer.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;


int main(int argc, char* argv[])
{
	using namespace WikiMainPath;

	auto output_folder = boost::filesystem::path(argv[1]);


	return 0;
}
