#include <algorithm>
#include <array>
#include <charconv>
#include <istream>
#include <ranges>
#include <vector>
#include <cassert>

#include <iostream>

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


	struct Number
	{
		std::int8_t n;

		constexpr auto operator<=>(const Number&) const = default;
	};


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


	struct Board
	{
		bool stripe(Number number)
		{
			const auto place = std::ranges::find(places, number, [](auto&& p) { return p.number; });
			if (place == places.end())
				return false;
			place->marked = true;
			const auto row = std::distance(places.begin(), place) / 5;
			const auto col = std::distance(places.begin(), place) % 5;
			return bingo_row(row) || bingo_col(col);
		}

		int sum_unmarked() const
		{
			return accumulate(
				places
					| std::views::filter([](auto&& p) { return !p.marked; })
					| std::views::transform([](auto&& p) { return p.number.n; }),
				0);
		}

		friend std::istream& operator>>(std::istream& is, Board& board)
		{
			for (auto i = 0; i < std::ssize(board.places); ++i)
			{
				int n;
				is >> n;
				board.places[i] = Board::Place{static_cast<Number>(n)};
			}
			return is;
		}

	private:
		bool bingo_row(int row) const
		{
			for (auto c = 0; c < 5; ++c)
				if (!places[5*row+c].marked)
					return false;
			return true;
		}
		bool bingo_col(int col) const
		{
			for (auto r = 0; r < 5; ++r)
				if (!places[5*r+col].marked)
					return false;
			return true;
		}

		struct Place { Number number; bool marked=false; };
		std::array<Place, 25> places;
	};


	auto read_numbers(std::istream& is)
	{
		std::vector<Number> numbers;
		auto first_line = std::string{};
		std::getline(is, first_line, '\n');
		std::ranges::move(
			first_line
				| std::views::split(',')
				| std::views::transform([](auto&& r) { return Number{from_chars<std::int8_t>(r)}; }),
			std::back_inserter(numbers));
		return numbers;
	}
}


int q04a(std::istream& is)
{
	const auto numbers = read_numbers(is);
	std::vector<Board> boards;
	std::ranges::move(std::ranges::istream_view<Board>(is), std::back_inserter(boards));
	for (auto number: numbers)
	{
		for (auto& board: boards)
			if (board.stripe(number))
				return number.n * board.sum_unmarked();
	}
	return -1;
}
