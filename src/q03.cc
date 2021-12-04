#include <algorithm>
#include <istream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>


namespace {


	struct BitwiseCount
	{
		std::vector<int> bit_count;
		int num_entries = 0;
	};


	BitwiseCount operator+(BitwiseCount lhs, const std::string& rhs)
	{
		if (lhs.num_entries == 0)
			lhs.bit_count.resize(rhs.size());
		else if (lhs.bit_count.size() != rhs.size())
			throw std::runtime_error("Unequal number of bits");
		for (auto i = 0; i < std::ssize(rhs); ++i)
			if (rhs[i] == '1')
				++lhs.bit_count[i];
		++lhs.num_entries;
		return lhs;
	}


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


}


int q03a(std::istream& is)
{
	const auto count = accumulate(std::ranges::istream_view<std::string>(is), BitwiseCount{});
	assert(count.bit_count.size() <= 31);
	auto gamma = 0;
	for (const auto& c: count.bit_count)
	{
		 gamma <<= 1;
		 gamma |= (2 * c) / count.num_entries;
	}
	auto epsilon = (1 << count.bit_count.size()) - 1 - gamma;
	return gamma * epsilon;
}
