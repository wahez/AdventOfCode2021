#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace {


	template<typename T>
	auto read_vector(std::istream& is)
	{
		std::vector<T> numbers;
		auto first_line = std::string{};
		std::getline(is, first_line, '\n');
		std::ranges::move(
			first_line
				| std::views::split(',')
				| std::views::transform([](auto&& r) { return from_chars<T>(r); }),
			std::back_inserter(numbers));
		return numbers;
	}


}


int q07a(std::istream& is)
{
	auto positions = read_vector<int>(is);
	std::ranges::nth_element(positions, positions.begin() + positions.size()/2);
	const auto position = positions[positions.size()/2];
	auto fuel_cost = [&](auto&& pos)
	{
		return std::abs(pos-position);
	};
	return accumulate(
		positions
			| std::views::transform(fuel_cost),
		0);
}


int q07b(std::istream& is)
{
	auto positions = read_vector<int>(is);
	auto total_fuel_cost = [&positions](const auto& pos)
	{
		auto fuel_cost = [&pos](const auto& p)
		{
			const auto distance = std::abs(pos-p);
			return ((distance+1) * distance) / 2;
		};
		return accumulate(
			positions | std::views::transform(fuel_cost),
			0ll);
	};
	// there is a global minimum, binary search for it
	const auto min_max = std::ranges::minmax_element(positions);
	auto min = *min_max.min;
	auto max = *min_max.max;
	while (min < max)
	{
		const auto position = min + (max - min) / 2;
		const auto fc1 = total_fuel_cost(position);
		const auto fc2 = total_fuel_cost(position+1);
		if (fc2 < fc1)
			min = position+1;
		else
			max = position;
	}
	return total_fuel_cost(min);
}
