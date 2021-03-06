#include <algorithm>
#include <array>
#include <istream>
#include <ranges>
#include "util.h"


namespace {


	template<typename T, int size>
	struct RotatableArray
	{
		T& operator[](int i) { return data[index(i)]; }

		void rotate() { start_index = index(1); }

		auto sum() const { return accumulate(data, T{0}); }

	private:
		int index(int i) const { i += start_index; return (i < size) ? i : i - size; }
		std::array<T, size> data{};
		int start_index{0}; // points to data at index 0
	};


	template<typename T, int size>
	auto read_fish(std::istream& is, RotatableArray<T, size>& population)
	{
		for (;;)
		{
			const auto timer = read<int>(is);
			++population[timer];
			switch (is.get())
			{
			case '\n':
				return;
			case ',':
				break;
			default:
				throw std::runtime_error("Could not parse population");
			}
		}
	}


}


int q06a(std::istream& is)
{
	auto population = RotatableArray<std::int64_t, 9>{};
	read_fish(is, population);
	for (auto i = 0; i < 80; ++i)
	{
		population.rotate();
		population[6] += population[8];
	}
	return population.sum();
}


std::int64_t q06b(std::istream& is)
{
	auto population = RotatableArray<std::int64_t, 9>{};
	read_fish(is, population);
	for (auto i = 0; i < 256; ++i)
	{
		population.rotate();
		population[6] += population[8];
	}
	return population.sum();
}
