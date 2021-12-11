#include <algorithm>
#include <iostream>
#include <ranges>
#include <vector>
#include "util.h"


namespace {


	struct LineParser
	{
		void process(char c)
		{
			if (first_error)
				return;
			if (!expected_close.empty() && c == expected_close.back())
				expected_close.pop_back();
			else if (const auto closing = get_closing_bracket(c); closing != 0)
				expected_close.push_back(closing);
			else
				first_error = c;
		}

		auto get_completion_score() const
		{
			auto score_for_char = [](const char c)
			{
				switch (c)
				{
				case ')': return 1;
				case ']': return 2;
				case '}': return 3;
				case '>': return 4;
				default: return 0;
				}
			};
			auto score = 0ll;
			for (const auto c: std::ranges::reverse_view(expected_close))
				score = score * 5 + score_for_char(c);
			return score;
		}

		auto get_first_error() const { return first_error; }

		static constexpr char get_closing_bracket(const char opening)
		{
			switch (opening)
			{
			case '(': return ')';
			case '[': return ']';
			case '{': return '}';
			case '<': return '>';
			default : return 0;
			}
		}

	private:
		std::vector<char> expected_close;
		char first_error = '\0';
	};


}


int q10a(std::istream& is)
{
	auto error_for_char = [](char c)
	{
		switch (c)
		{
		case ')': return 3;
		case ']': return 57;
		case '}': return 1197;
		case '>': return 25137;
		default : return 0;
		}
	};
	auto error_score = 0;
	std::noskipws(is);
	while (is.good())
	{
		auto parser = LineParser{};
		auto line = std::ranges::istream_view<char>(is) | std::views::take_while([](char c) { return c != '\n'; });
		for (auto c: line)
			parser.process(c);
		error_score += error_for_char(parser.get_first_error());
	}
	return error_score;
}


std::int64_t q10b(std::istream& is)
{
	auto scores = std::vector<std::int64_t>{};
	std::noskipws(is);
	while (is.good())
	{
		auto parser = LineParser{};
		auto line = std::ranges::istream_view<char>(is) | std::views::take_while([](char c) { return c != '\n'; });
		for (auto c: line)
			parser.process(c);
		if (parser.get_first_error() == 0)
			scores.push_back(parser.get_completion_score());
	}
	std::ranges::sort(scores);
	return scores[scores.size()/2];
}
