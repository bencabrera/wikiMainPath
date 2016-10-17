#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <random>
#include <array>
#include <numeric>


int main()
{
	constexpr std::size_t num_mutices = 3;
	constexpr std::size_t num_entries = 6;
	constexpr std::size_t num_threads = 4;

	std::array<std::mutex, num_mutices> mutices;
	std::array<std::size_t, num_entries> data;

	// fill data with 1, 2, ...
	std::iota(data.begin(), data.end(), 1);

	// setup random generators
	std::random_device rd;
	std::vector<std::mt19937> rand_gens;
	for(std::size_t i = 0; i < num_threads; i++)
		rand_gens.push_back(std::mt19937(rd()));

	std::vector<std::thread> threads;
	for(std::size_t iThread = 0; iThread < num_threads; iThread++)
	{
		threads.emplace_back(std::thread([&data, &mutices, &rand_gens, iThread]()
		{
			// distribution for picking a random value from data
			std::uniform_int_distribution<std::size_t> dist(0,data.size()-1);	

			for(std::size_t a = 0; a < 100; a++)
			{
				auto iFirst = dist(rand_gens[iThread]);
				auto iSecond = dist(rand_gens[iThread]);

				auto iMin = std::min(iFirst % mutices.size(), iSecond % mutices.size());
				auto iMax = std::max(iFirst % mutices.size(), iSecond % mutices.size());

				if(iMin == iMax)
					continue;

				std::cout << iMin << " " << iMax << std::endl;

				std::lock_guard<std::mutex> lock1(mutices[iMin]);
				std::lock_guard<std::mutex> lock2(mutices[iMax]);

				std::size_t tmp = data[iFirst];
				data[iFirst] = data[iSecond];
				data[iSecond] = tmp;
			}
		}));
	}

	for(std::size_t i = 0; i < num_threads; i++)
		threads[i].join();
}
