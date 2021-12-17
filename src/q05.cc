#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace {


	struct Point
	{
		int x;
		int y;

		auto operator<=>(const Point&) const = default;

		auto operator+(Point rhs) const
		{
			return Point{x + rhs.x, y + rhs.y};
		}
	};

	std::istream& operator>>(std::istream& is, Point& point)
	{
		is >> point.x >> Assert{','} >> point.y;
		return is;
	}


	struct Line
	{
		Point start;
		Point end;

		auto direction() const
		{
			return Point{direction(start.x, end.x), direction(start.y, end.y)};
		}

	private:
		static int direction(int start, int end)
		{
			return (end > start) ? 1 : (end < start ? -1 : 0);
		}
	};

	std::istream& operator>>(std::istream& is, Line& line)
	{
		is >> line.start >> Assert(" ->") >> line.end;
		return is;
	}


	template<int size>
	struct Board
	{
		explicit Board():
			point_usage(size*size, 0)
		{
		}

		void mark(const Line& line)
		{
			if (line.start.x < 0 ||
			    line.end.x < 0 ||
			    line.start.y < 0 ||
			    line.end.y < 0 ||
			    line.start.x >= size ||
			    line.end.x >= size ||
			    line.start.y >= size ||
			    line.end.y >= size)
				throw std::runtime_error("Point out of bounds");
			const auto direction = line.direction();
			const auto end = line.end + direction;
			for (auto current = line.start; current != end; current = current + direction)
				++point_usage[current.x + size*current.y];
		}

		int count_squares_used_multiple_times() const
		{
			return std::ranges::count_if(
				point_usage,
				[](auto&& e) { return e > 1; });
		}

	private:
		std::vector<std::int8_t> point_usage;
	};


}


int q05a(std::istream& is)
{
	auto board = Board<1024>{};
	std::ranges::for_each(
		std::ranges::istream_view<Line>(is)
		| std::views::filter([](auto&& e) { const auto d = e.direction(); return d.x == 0 || d.y == 0; }),
		[&](auto&& line) { board.mark(line); });
	return board.count_squares_used_multiple_times();
}


int q05b(std::istream& is)
{
	auto board = Board<1024>{};
	std::ranges::for_each(
		std::ranges::istream_view<Line>(is),
		[&](auto&& line) { board.mark(line); });
	return board.count_squares_used_multiple_times();
}
