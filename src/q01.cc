#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>


namespace
{


	void for_each_zipped(auto&& range1, auto&& range2, auto&& pred)
	{
		using std::begin;
		using std::end;
		auto c1 = begin(range1);
		auto c2 = begin(range2);
		for (; c1 != end(range1) && c2 != end(range2); ++c1, ++c2)
			pred(*c1, *c2);
	}


}


int q01a(std::istream& is)
{
	auto count = 0;
	auto input = std::ranges::istream_view<int>(is);
	auto current = input.begin();
	if (current == input.end())
		throw std::runtime_error("Not enough values");
	// go through the list without storing it (so no allocation)
	auto previous = *current;
	++current;
	for (; current != input.end(); ++current)
	{
		if (*current > previous)
			++count;
		previous = *current;
	}
	return count;
}


int q01b(std::istream& is)
{
	auto numbers = std::vector<int>{};
	std::ranges::move(
			std::ranges::istream_view<int>(is),
			std::back_inserter(numbers));
	if (numbers.size() < 4)
		throw std::runtime_error("Not enough values");
	// only check if the number added to the sliding window is bigger than the number
	auto numbers_offset = numbers | std::views::drop(3);
	auto count = 0;
	// zip_view is not in std yet
	for_each_zipped(numbers, numbers_offset, [&](const auto& l, const auto& r) { if (r > l) ++count; });
	return count;
}
