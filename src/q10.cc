#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace {


	struct LineAnalyzer
	{
		bool process(char c)
		{
			if (!expected_close.empty() && c == expected_close.back())
			{
				expected_close.pop_back();
				return true;
			}
			switch (c)
			{
			case '(':
				expected_close.push_back(')');
				return true;
			case '[':
				expected_close.push_back(']');
				return true;
			case '{':
				expected_close.push_back('}');
				return true;
			case '<':
				expected_close.push_back('>');
				return true;
			}
			return false;
		}

	private:
		std::vector<char> expected_close;
	};


}


int q10a(std::istream& is)
{
	auto error_score = 0;
	while (is.good())
	{
		auto analyzer = LineAnalyzer{};
		while (is.good())
		{
			const auto c = is.get();
			if (analyzer.process(c))
				continue;
			switch (c)
			{
			case ')':
				error_score += 3;
				break;
			case ']':
				error_score += 57;
				break;
			case '}':
				error_score += 1197;
				break;
			case '>':
				error_score += 25137;
				break;
			}
			if (c != '\n')
				while (is.good() && is.get() != '\n') {}
			break;
		}
	}
	return error_score;
}
