#include <algorithm>
#include <istream>
#include <ranges>
#include <string>
#include <vector>


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


	struct Display
	{
		std::vector<std::string> digits;
	};

	std::istream& operator>>(std::istream& is, Display& display)
	{
		display.digits.clear();
		auto line = std::string{};
		std::getline(is, line, '|');
		is.get(); // remove space
		std::getline(is, line, '\n');
		std::ranges::move(
			line
				| std::views::split(' ')
				| std::views::transform([](auto&& r) { return std::string(&*r.begin(), std::ranges::distance(r)); }),
			std::back_inserter(display.digits));
		return is;
	}


}


int q08a(std::istream& is)
{
	auto count_unique_characters = [](const Display& display)
	{
		return std::ranges::count_if(
			display.digits,
			[](const auto& d)
			{
				const auto size = d.size();
				return size == 2 || size == 4 || size == 3 || size == 7;
			});
	};
	return
		accumulate(
			std::ranges::istream_view<Display>(is)
				| std::views::transform(count_unique_characters),
			0);
}
