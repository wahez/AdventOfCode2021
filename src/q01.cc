#include <istream>
#include <optional>
#include <ranges>


int q01a(std::istream& is)
{
	auto count = 0;
	auto input = std::ranges::istream_view<int>(is);
	auto current = input.begin();
	if (current != input.end())
	{
		auto previous = *current;
		++current;
		for (; current != input.end(); ++current)
		{
			if (*current > previous)
				++count;
			previous = *current;
		}
	}
	return count;
}
