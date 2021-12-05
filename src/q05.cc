#include <algorithm>
#include <istream>
#include <map>
#include <ranges>


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
		is >> point.x;
		if (is.good() && is.get() != ',')
			throw std::runtime_error("Could not parse point");
		is >> point.y;
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
		is >> line.start;
		std::string arrow;
		is >> arrow;
		if (is.good() && arrow != "->")
			throw std::runtime_error("Could not parse line");
		is >> line.end;
		return is;
	}


	struct Board
	{
		void mark(const Line& line)
		{
			const auto direction = line.direction();
			const auto end = line.end + direction;
			for (auto current = line.start; current != end; current = current + direction)
				++point_usage[current];
		}

		int count_squares_used_multiple_times() const
		{
			return std::ranges::count_if(
				point_usage,
				[](auto&& e) { return e.second > 1; });
		}

	private:
		std::map<Point, int> point_usage;
	};


}


int q05a(std::istream& is)
{
	auto board = Board{};
	std::ranges::for_each(
		std::ranges::istream_view<Line>(is)
		| std::views::filter([](auto&& e) { const auto d = e.direction(); return d.x == 0 || d.y == 0; }),
		[&](auto&& line) { board.mark(line); });
	return board.count_squares_used_multiple_times();
}
