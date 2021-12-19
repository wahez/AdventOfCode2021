#include <array>
#include <algorithm>
#include <istream>
#include <map>
#include <ranges>
#include <string>
#include "util.h"


namespace
{

	struct Rules
	{
		char operator()(char a, char b) const
		{
			return rules[(a-'A') + (b-'A')*26];
		}
		char& operator()(char a, char b)
		{
			return rules[(a-'A') + (b-'A')*26];
		}
	private:
		std::array<char, 26*26> rules;
	};

	std::istream& operator>>(std::istream& is, Rules& rules)
	{
		while (is.peek() != '\n')
		{
			char a, b, c;
			is >> a >> b >> Assert(" -> ") >> c >> Assert('\n');
			if (!is.good())
				break;
			rules(a, b) = c;
		}
		return is;
	}


	struct Polymer
	{
		explicit Polymer(const std::string& polymer) :
			first(polymer.front()),
			last(polymer.back())
		{
			for (auto i = 0u; i < polymer.size()-1; ++i)
				++pairs[{polymer[i], polymer[i+1]}];
		}

		void grow(const Rules& rules)
		{
			auto result = decltype(pairs){};
			for (const auto& [pair, count]: pairs)
			{
				const auto middle = rules(pair[0], pair[1]);
				result[{pair[0], middle}] += count;
				result[{middle, pair[1]}] += count;
			}
			std::swap(result, pairs);
		}

		auto count_occurrences() const
		{
			auto result = std::map<char, std::int64_t>{};
			for (const auto& [pair, count]: pairs)
			{
				result[pair[0]] += count;
				result[pair[1]] += count;
			}
			// all letters are in two pairs except the ends
			++result[first];
			++result[last];
			// divide by two
			for (auto& [c, count]: result)
				count /= 2;
			return result;
		}
	private:
		std::map<std::array<char, 2>, std::int64_t> pairs;
		const char first;
		const char last;
	};

}


int q14a(std::istream& is)
{
	auto polymer_input = std::string{};
	auto rules = Rules{};
	is >> polymer_input >> Assert("\n\n") >> rules;
	auto polymer = Polymer{std::move(polymer_input)};
	for (auto i = 0; i < 10; ++i)
		polymer.grow(rules);
	auto occurrences = polymer.count_occurrences();
	const auto& [min, max] = std::ranges::minmax_element(occurrences, std::ranges::less{}, &decltype(occurrences)::value_type::second);
	return max->second - min->second;
}


std::int64_t q14b(std::istream& is)
{
	auto polymer_input = std::string{};
	auto rules = Rules{};
	is >> polymer_input >> Assert("\n\n") >> rules;
	auto polymer = Polymer{std::move(polymer_input)};
	for (auto i = 0; i < 40; ++i)
		polymer.grow(rules);
	auto occurrences = polymer.count_occurrences();
	const auto& [min, max] = std::ranges::minmax_element(occurrences, std::ranges::less{}, &decltype(occurrences)::value_type::second);
	return max->second - min->second;
}
