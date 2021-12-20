#include <algorithm>
#include <istream>
#include <limits>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace
{


	struct Pos
	{
		int x;
		int y;

		auto operator<=>(const Pos&) const = default;
		Pos operator+(const Pos& rhs) const { return Pos{x+rhs.x, y+rhs.y}; }
	};


	template<typename T>
	struct Grid
	{
		Grid(Pos size, T value) :
			row_length(size.x),
			cells(to_index(size)-row_length, value)
		{
		}

		Grid(Pos size, std::vector<T> data) :
			row_length(size.x),
			cells(std::move(data))
		{
			cells.resize(to_index(size)-row_length, T{});
		}

		T const& operator[](Pos pos) const { return cells[to_index(pos)]; }
		T      & operator[](Pos pos)       { return cells[to_index(pos)]; }
		Pos end() const { return Pos{row_length, static_cast<int>(cells.size())/row_length}; }

	private:
		int to_index(Pos pos) const { return row_length * pos.y + pos.x; }
		const int row_length;
		std::vector<T> cells;
	};

	struct CavernSolver
	{
		Grid<std::int8_t> cells;

	private:
		void solve_neighbours(Pos pos, Grid<int>& cost, const Pos end) const
		{
			static constexpr Pos all_directions[] = {Pos{0,1}, Pos{0,-1}, Pos{1,0}, Pos{-1,0}};
			for (auto& dir: all_directions)
			{
				const auto new_pos = pos+dir;
				const auto new_cost = cost[pos] + cells[new_pos];
				const auto old_cost = cost[new_pos];
				if (new_cost < old_cost && cells[new_pos] < 10)
				{
					cost[new_pos] = new_cost;
					if (new_pos != end && old_cost < std::numeric_limits<int>::max())
						solve_neighbours(new_pos, cost, end);
				}
			}
		}

	public:
		auto solve() const
		{
			const auto end = cells.end() + Pos{-1, -1};
			auto cost = Grid<int>(cells.end(), std::numeric_limits<int>::max());
			cost[Pos{1, 1}] = 0;
			for(auto y = 1; y < end.y; ++y)
				for(auto x = 1; x < end.x; ++x)
					solve_neighbours(Pos{x, y}, cost, end);
			return cost[end+Pos{-1,-1}];
		}

		static auto read(std::istream& is)
		{
			auto result = std::vector<std::int8_t>{};
			auto lines = 0;
			std::noskipws(is);
			while (is.good() && is.peek() != '\n' && is.peek() != EOF)
			{
				result.push_back(127);
				std::ranges::copy(
					std::ranges::istream_view<char>(is)
					 | std::views::take_while([](char c) { return c != '\n'; })
					 | std::views::transform([](char c) { return c-'0'; }),
					std::back_inserter(result));
				result.push_back(127);
				++lines;
			}
			const auto line_size = static_cast<int>(result.size() / lines);
			result.insert(result.begin(), line_size, 127);
			result.insert(result.end(), line_size, 127);
			lines += 2;
			assert(line_size * lines == std::ssize(result));
			return Grid<std::int8_t>{{line_size, lines}, std::move(result)};
		}
	};


}


int q15a(std::istream& is)
{
	const auto grid = CavernSolver::read(is);
	auto solver = CavernSolver{grid};
	const auto total_risk = solver.solve();
	return total_risk;
}
