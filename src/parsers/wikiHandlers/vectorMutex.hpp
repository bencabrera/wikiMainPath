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

		void unlock(std::size_t idx)
		{
			return _mutices.at(idx % _mutices.size()).unlock();
		}

	private:
		std::array<std::mutex, TNumMutices> _mutices;
};
