#include <algorithm>
#include <array>
#include <bitset>
#include <istream>
#include <ranges>
#include <string>
#include <cassert>
#include "util.h"


namespace {


	using Segments = std::bitset<7>;

	auto read_segments(std::istream& is)
	{
		auto segments = Segments{};
		for (auto next = is.peek(); next >= 'a' && next <= 'g'; is.get(), next = is.peek())
			segments.set(next - 'a');
		return segments;
	}


	struct Display
	{
		std::array<Segments, 10> scrambled_wires_for_digits;
		std::array<Segments, 4> displayed_digits;

		auto digit_value(const std::array<Segments, 10>& correct_wiring) const
		{
			auto value = 0;
			for (const auto& d: displayed_digits)
			{
				value *= 10;
				const auto it = std::ranges::find(correct_wiring, d);
				value += std::distance(correct_wiring.begin(), it);
			}
			return value;
		}
	};

	std::istream& operator>>(std::istream& is, Display& display)
	{
		for (auto& wiring: display.scrambled_wires_for_digits)
		{
			wiring = read_segments(is);
			if (is.get() != ' ' && is.good())
				throw std::runtime_error("Expected ' ' in wiring");
		}
		if ((is.get() != '|' || is.get() != ' ') && is.good())
			throw std::runtime_error("Expected '| '");
		for (auto& digits: display.displayed_digits)
		{
			digits = read_segments(is);
			const auto next = is.get();
			if (next != ' ' && next != '\n' && is.good())
				throw std::runtime_error("Expected ' ' in digits");
		}
		return is;
	}


	std::array<Segments, 10> solve(const std::array<Segments, 10>& problem)
	{
		std::array<Segments, 10> solution;
		solution[1] = *std::ranges::find_if(problem, [](const auto& s) { return s.count() == 2; });
		solution[4] = *std::ranges::find_if(problem, [](const auto& s) { return s.count() == 4; });
		solution[7] = *std::ranges::find_if(problem, [](const auto& s) { return s.count() == 3; });
		solution[8] = *std::ranges::find_if(problem, [](const auto& s) { return s.count() == 7; });
		solution[3] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 5 && (s & solution[1]) == solution[1]; });
		solution[6] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 6 && (s & solution[1]) != solution[1]; });
		solution[9] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 6 && (s & solution[4]) == solution[4]; });
		solution[2] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 5 && (s | solution[9]).count() == 7; });
		solution[5] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 5 && (s | solution[6]).count() == 6; });
		solution[0] = *std::ranges::find_if(problem, [&](const auto& s) { return s.count() == 6 && (s & solution[5]) != solution[5]; });
		return solution;
	}


}


int q08a(std::istream& is)
{
	auto count_unique_characters = [](const Display& display)
	{
		return std::ranges::count_if(
			display.displayed_digits,
			[](const auto& d)
			{
				const auto size = d.count();
				return size == 2 || size == 4 || size == 3 || size == 7;
			});
	};
	return
		accumulate(
			std::ranges::istream_view<Display>(is)
				| std::views::transform(count_unique_characters),
			0);
}


int q08b(std::istream& is)
{
	return accumulate(
		std::ranges::istream_view<Display>(is)
			| std::views::transform(
				[](const Display& display)
				{
					const auto solution = solve(display.scrambled_wires_for_digits);
					return display.digit_value(solution);
				}),
		0);
}
