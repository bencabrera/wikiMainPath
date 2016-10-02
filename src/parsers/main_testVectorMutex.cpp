#include <iostream>
#include <thread>
#include <set>
#include <vector>
#include <chrono>

#include "wikiHandlers/vectorMutex.hpp"

#include <random>


int main()
{
	std::vector<std::set<std::size_t>> vec;
	VectorMutex<2> vecMutex;

	constexpr std::size_t vecLength = 1;
	constexpr std::size_t numThreads = 5;

	for(std::size_t i = 0; i < vecLength; i++)
		vec.push_back(std::set<std::size_t>());

	std::random_device rd;

	std::vector<std::mt19937> rand_gens;
	for(std::size_t i = 0; i < numThreads; i++)
		rand_gens.push_back(std::mt19937(rd()));


	std::vector<std::thread> threads;
	for(std::size_t i = 0; i < numThreads; i++)
	{
		threads.emplace_back(std::thread([&vec, &vecMutex, &rand_gens, i, &vecLength]()
		{
			std::size_t iThread = i;
			std::uniform_int_distribution<std::size_t> dist(0,100);	

			for(std::size_t a = 0; a < 5; a++)
			{
				std::size_t vec_i = dist(rand_gens[i]) % vecLength;
				std::cout << "vec_i: " << vec_i << std::endl;
				vecMutex.lock(vec_i);
				vec[vec_i].insert(a + iThread * 1000);
				std::cout << "Thread " << iThread << " added val to vec[" << vec_i << "]!" << std::endl;
				std::this_thread::sleep_for(std::chrono::seconds{1});
				vecMutex.unlock(vec_i);
			}


		}));
	}

	for(std::size_t i = 0; i < numThreads; i++)
		threads[i].join();

	std::size_t total = 0;
	for (auto i : vec) {
		total += i.size();
		for (auto v : i) {
			std::cout << v << " ";
		}	
		std::cout << std::endl;
	}
	std::cout << "Total: " << total << std::endl;
}
