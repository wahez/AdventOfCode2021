#include <algorithm>
#include <charconv>
#include <ranges>


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
