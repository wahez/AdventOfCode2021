#include <array>
#include <algorithm>
#include <istream>
#include <optional>
#include <ranges>
#include <vector>
#include "util.h"


namespace
{


	template<int LEVELS>
	struct SnailNumber
	{
	public:
		explicit SnailNumber()
		{
			numbers.fill(-1);
		}

		int magnitude() const
		{
			auto magnitudes = std::array<int, SIZE>{};
			std::ranges::copy(numbers, magnitudes.begin());
			for (auto end = SIZE/2; end > 0; end /= 2)
				for (auto c = 0; c < end; ++c)
					if (magnitudes[2*c+1] == -1)
						magnitudes[c] = magnitudes[2*c];
					else
						magnitudes[c] = 3*magnitudes[2*c] + 2*magnitudes[2*c+1];
			return magnitudes[0];
		}

		const auto& operator[](int i) const { return numbers[i]; }
		      auto& operator[](int i)       { return numbers[i]; }

		SnailNumber<LEVELS> operator+(const SnailNumber<LEVELS>& rhs) const
		{
			auto snail_number = SnailNumber<LEVELS+1>{};
			auto copy_result = std::ranges::copy(numbers, snail_number.numbers.begin());
			std::ranges::copy(rhs.numbers, copy_result.out);
			do
			{
				snail_number.explode();
			}
			while (snail_number.split());
			auto lower_level = SnailNumber<LEVELS>{};
			for (auto i = 0; i < SIZE; ++i)
				lower_level[i] = snail_number[2*i];
			return lower_level;
		}

	private:
		static constexpr auto SIZE = 1 << LEVELS;
		std::array<std::int8_t, SIZE> numbers;

		void explode()
		{
			int carry_right = 0;
			for (auto i = 1; i < SIZE; ++i)
				if (numbers[i] != -1)
				{
					if (i & 1)
					{
						const auto carry_left = std::exchange(numbers[i-1], 0) + carry_right;
						carry_right = std::exchange(numbers[i], -1);
						for (auto l = i-3; l >= 0; l -= 2)
							if (numbers[l] != -1)
							{
								numbers[l] += carry_left;
								break;
							}
					}
					else
						numbers[i] += std::exchange(carry_right, 0);
				}
		}

		bool split()
		{
			for (auto i = 0; i < std::ssize(numbers); i += 2)
				if (numbers[i] > 9)
				{
					for (auto step = 2; step < (1<<LEVELS); step *= 2)
						if ((i & step) || numbers[i+step] != -1)
						{
							numbers[i+step/2] = (numbers[i]+1)/2;
							numbers[i] = numbers[i]/2;
							break;
						}
					return true;
				}
			return false;
		}
		template<int L> friend struct SnailNumber;
	};


	template<int LEVELS>
	std::ostream& operator<<(std::ostream& os, SnailNumber<LEVELS>& snail_number)
	{
		auto print_recursive = [&](auto index, auto step, auto&& next) -> void
		{
			if (step == 0 || snail_number[index+step] == -1)
				os << NotChar(snail_number[index]);
			else
			{
				os << '[';
				next(index, step/2, next);
				os << ',';
				next(index+step, step/2, next);
				os << ']';
			}
		};
		print_recursive(0, 1<<(LEVELS-1), print_recursive);
		return os;
	}

	template<int LEVELS>
	std::istream& operator>>(std::istream& is, SnailNumber<LEVELS>& snail_number)
	{
		snail_number = SnailNumber<LEVELS>{};
		auto step = 1<<(LEVELS-1);
		auto index = 0;
		is >> Assert('[');
		while (is.good() && step < (1<<LEVELS))
		{
			const char c = is.peek();
			if (c == '[')
			{
				is.get();
				step /= 2;
			}
			else if (c == ',')
			{
				is.get();
				index += step;
			}
			else if (c == ']')
			{
				is.get();
				index -= step;
				step *= 2;
			}
			else
				snail_number[index] = read<int>(is);
		}
		return is;
	}


}


int q18a(std::istream& is)
{
	auto numbers = std::ranges::istream_view<SnailNumber<4>>(is);
	auto it = numbers.begin();
	auto total = std::move(*it);
	is >> Assert('\n');
	for (++it; it != numbers.end(); ++it)
	{
		total = std::move(total) + std::move(*it);
		is >> Assert('\n');
		if (is.peek() == '\n')
			break;
	}
	return total.magnitude();
}


int q18b(std::istream& is)
{
	auto numbers = std::vector<SnailNumber<4>>{};
	for (auto& sn: std::ranges::istream_view<SnailNumber<4>>(is))
	{
		numbers.push_back(std::move(sn));
		is >> Assert('\n');
		if (is.peek() == '\n')
			break;
	}
	auto max = 0;
	for (auto i1 = 0; i1 < std::ssize(numbers); ++i1)
		for (auto i2 = 0; i2 < std::ssize(numbers); ++i2)
			if (i1 != i2)
				max = std::max(max, (numbers[i1]+numbers[i2]).magnitude());
	return max;
}
