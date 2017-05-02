#pragma once

#include <mutex>
#include <array>

template<int TNumMutices>
class VectorMutex
{
	public:
		void lock(std::size_t idx)
		{
			return _mutices.at(idx % _mutices.size()).lock();
		}

		bool try_lock(std::size_t idx)
		{
			return _mutices.at(idx % _mutices.size()).try_lock();
		}

		std::mutex& get(std::size_t idx)
		{
			return _mutices.at(idx % _mutices.size());
		}

		void unlock(std::size_t idx)
		{
			return _mutices.at(idx % _mutices.size()).unlock();
		}

	private:
		std::array<std::mutex, TNumMutices> _mutices;
};
