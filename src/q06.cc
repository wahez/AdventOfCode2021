#include <algorithm>
#include <array>
#include <charconv>
#include <istream>
#include <ranges>


namespace {


	template<typename Op = std::plus<>>
	auto accumulate(std::ranges::input_range auto&& range, auto init, Op&& op = Op{})
	{
		std::ranges::for_each(
			range,
			[&](const auto& element)
			{
				init = std::move(init) + element;
			});
		return init;
	}


	struct Population
	{
		void add_fish(int timer)
		{
			++population_by_timer[timer];
		}

		void evolve()
		{
			// population moves one timer value down
			// all fish that were at timer 0 move to 6
			// and spawn new fish at 8
			std::ranges::rotate(population_by_timer, population_by_timer.begin() + 1);
			population_by_timer[6] += population_by_timer[8];
		}

		auto population() const
		{
			return accumulate(population_by_timer, 0);
		}

	private:
		std::array<int, 9> population_by_timer;
	};


	template<typename T>
	T from_chars(auto&& range)
	{
		auto value = T{};
		const auto first = &*range.begin();
		const auto last = first + std::ranges::distance(range);
		const auto result = std::from_chars(first, last, value);
		if (result.ptr != last)
			throw std::runtime_error("Could not parse");
		return value;
	}


}


int q06a(std::istream& is)
{
	auto population = Population{};
	auto line = std::string{};
	std::getline(is, line, '\n');
	std::ranges::for_each(
		line | std::views::split(','),
		[&](auto&& r)
		{
			const auto timer = from_chars<int>(r);
			population.add_fish(timer);
		});
	for (auto i = 0; i < 80; ++i)
		population.evolve();
	return population.population();
}
