#include <algorithm>
#include <array>
#include <bitset>
#include <istream>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace {


	struct Number
	{
		std::int8_t n;

		constexpr auto operator<=>(const Number&) const = default;
	};

	std::istream& operator>>(std::istream& is, Number& number)
	{
		is >> NotChar(number.n);
		return is;
	}


	struct Board
	{
		bool stripe(Number number)
		{
			const auto place = std::ranges::find(places, number);
			if (place == places.end())
				return false;
			const auto index = std::distance(places.begin(), place);
			marked.set(index);
			const auto row = index / 5;
			const auto col = index % 5;
			return bingo_row(row) || bingo_col(col);
		}

		int sum_unmarked() const
		{
			auto sum = 0;
			for (auto i = 0; i < std::ssize(places); ++i)
				if (!marked.test(i))
					sum += places[i].n;
			return sum;
		}

		friend std::istream& operator>>(std::istream& is, Board& board)
		{
			for (auto i = 0; i < std::ssize(board.places); ++i)
				is >> NotChar(board.places[i].n);
			return is;
		}

	private:
		bool bingo_row(int row) const
		{
			constexpr auto single_row = 0b11111;
			return ((marked >> (5*row)).to_ulong() & single_row) == single_row;
		}
		bool bingo_col(int col) const
		{
			constexpr auto single_col = 0b00001'00001'00001'00001'00001;
			return ((marked >> col).to_ulong() & single_col) == single_col;
		}

		std::array<Number, 25> places;
		std::bitset<25> marked;
	};


}


int q04a(std::istream& is)
{
	const auto numbers = read_separated<std::vector<Number>>(is);
	auto boards = read_non_separated<std::vector<Board>>(is);
	for (auto number: numbers)
	{
		for (auto& board: boards)
			if (board.stripe(number))
				// first board that wins, return score
				return number.n * board.sum_unmarked();
	}
	return -1;
}


int q04b(std::istream& is)
{
	const auto numbers = read_separated<std::vector<Number>>(is);
	auto boards = read_non_separated<std::vector<Board>>(is);
	for (auto number: numbers)
	{
		for (auto it = boards.begin(); it != boards.end();)
		{
			if (it->stripe(number))
			{
				if (boards.size() == 1)
					// last board that wins, return score
					return number.n * it->sum_unmarked();
				else
					it = boards.erase(it);
			}
			else
				++it;
		}
	}
	return -1;
}
