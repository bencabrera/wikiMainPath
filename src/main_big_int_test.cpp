#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>
#include <limits>

int main(int argc, char *argv[])
{
	using type = boost::multiprecision::cpp_dec_float_100;
	type var;

	var = 1;
	for(int i = 0; i < 20; i++)
		var *= 100;

	std::cout << var << std::endl;
	std::cout << boost::multiprecision::log(var) << std::endl;
	std::cout << std::numeric_limits<type>::max() << std::endl;

	return 0;
}
