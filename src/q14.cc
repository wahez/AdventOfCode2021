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
		void grow(const Rules& rules)
		{
			auto result = std::string(2*polymer.size()-1, '0');
			for (auto i = 0u; i < polymer.size()-1; ++i)
			{
				result[2*i] = polymer[i];
				result[2*i+1] = rules(polymer[i], polymer[i+1]);
			}
			result.back() = polymer.back();
			std::swap(polymer, result);
		}
		auto count_occurrences() const
		{
			auto result = std::map<char, std::int64_t>{};
			for (const char c: polymer)
				++result[c];
			return result;
		}

		std::string polymer;
	};

	std::istream& operator>>(std::istream& is, Polymer& polymer)
	{
		std::getline(is, polymer.polymer);
		return is;
	}

}


int q14a(std::istream& is)
{
	auto polymer = Polymer{};
	auto rules = Rules{};
	is >> polymer >> Assert('\n') >> rules;
	for (auto i = 0; i < 10; ++i)
		polymer.grow(rules);
	auto occurrences = polymer.count_occurrences();
	const auto& [min, max] = std::ranges::minmax_element(occurrences, std::ranges::less{}, &decltype(occurrences)::value_type::second);
	return max->second - min->second;
}
