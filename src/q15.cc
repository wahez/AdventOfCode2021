#include <algorithm>
#include <istream>
#include <limits>
#include <queue>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace
{


	struct Pos
	{
		explicit Pos(int xn, int yn) : x(xn), y(yn) {}
		std::int16_t x;
		std::int16_t y;

		auto operator<=>(const Pos&) const = default;
		Pos operator+(const Pos& rhs) const { return Pos{x+rhs.x, y+rhs.y}; }
		Pos operator*(int i) const { return Pos{x*i, y*i}; }
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
		T const& operator[](int index) const { return cells[index]; }
		T      & operator[](int index)       { return cells[index]; }
		Pos end() const { return Pos{row_length, static_cast<int>(cells.size())/row_length}; }

		int to_index(Pos pos) const { return row_length * pos.y + pos.x; }
	private:
		int row_length;
		std::vector<T> cells;
	};


	struct CavernSolver
	{
		using RiskLevel = std::int16_t;
		static constexpr auto border_level = std::int8_t{127};
		static constexpr auto unknown_risk_level = std::numeric_limits<RiskLevel>::max();

		explicit CavernSolver(Grid<std::int8_t> grid) :
			positions(Pos{grid.end().x+2, grid.end().y+2}, {})
		{
			// add border
			for (auto y = 0; y < grid.end().y; ++y)
				for (auto x = 0; x < grid.end().x; ++x)
					positions[Pos{x+1, y+1}] = grid[Pos{x, y}];
			// set border to high
			for (auto y = 0; y < positions.end().y; ++y)
			{
				positions[Pos{0,y}] = border_level;
				positions[Pos{positions.end().x-1,y}] = border_level;
			}
			for (auto x = 1; x < positions.end().x; ++x)
			{
				positions[Pos{x,0}] = border_level;
				positions[Pos{x,positions.end().y-1}] = border_level;
			}
		}

	private:
		Grid<std::int8_t> positions;

	public:
		struct PendingPos
		{
			RiskLevel total_risk_level;
			int pos;
			auto operator<=>(const PendingPos& rhs) const { return total_risk_level <=> rhs.total_risk_level; }
		};

		auto solve()
		{
			const auto all_directions = std::array<int, 4>{1, -1, positions.end().x, -positions.end().x};
			auto total_risk_level = Grid<RiskLevel>(positions.end(), unknown_risk_level);
			auto to_calculate = std::priority_queue<PendingPos, std::vector<PendingPos>, std::greater<>>{};
			to_calculate.push({0, positions.to_index(Pos{1, 1})});
			total_risk_level[positions.to_index(Pos{1,1})] = 0;
			while (!to_calculate.empty())
			{
				const auto risk_pos = to_calculate.top();
				to_calculate.pop();
				if (total_risk_level[risk_pos.pos] == risk_pos.total_risk_level)
					for (const auto& dir: all_directions)
					{
						const auto new_pos = risk_pos.pos+dir;
						if (const auto risk_level = positions[new_pos]; risk_level != border_level)
						{
							const RiskLevel new_total_risk_level = risk_pos.total_risk_level + risk_level;
							if (new_total_risk_level < total_risk_level[new_pos])
							{
								to_calculate.push({new_total_risk_level, new_pos});
								total_risk_level[new_pos] = new_total_risk_level;
							}
						}
					}
			}
			return total_risk_level[positions.end()+Pos{-2,-2}];
		}
	};


	auto read_grid(std::istream& is)
	{
		auto result = std::vector<std::int8_t>{};
		auto lines = 0;
		std::noskipws(is);
		while (is.good() && is.peek() != '\n' && is.peek() != EOF)
		{
			std::ranges::copy(
				std::ranges::istream_view<char>(is)
				 | std::views::take_while([](char c) { return c != '\n'; })
				 | std::views::transform([](char c) { return c-'0'; }),
				std::back_inserter(result));
			++lines;
		}
		const auto line_size = static_cast<int>(result.size() / lines);
		assert(line_size * lines == std::ssize(result));
		return Grid<std::int8_t>{Pos{line_size, lines}, std::move(result)};
	}


	template<typename T>
	auto multiply(const Grid<T>& grid, int factor)
	{
		auto result = Grid<T>{grid.end()*factor, {}};
		for (auto yi = 0; yi < factor; ++yi)
			for (auto xi = 0; xi < factor; ++xi)
				for (auto y = 0; y < grid.end().y; ++y)
					for (auto x = 0; x < grid.end().x; ++x)
						result[Pos{grid.end().x*xi, grid.end().y*yi} + Pos{x,y}] = (grid[Pos{x, y}] + xi + yi - 1) % 9 + 1;
		return result;
	}


}


int q15a(std::istream& is)
{
	const auto grid = read_grid(is);
	auto solver = CavernSolver{grid};
	const auto total_risk = solver.solve();
	return total_risk;
}


int q15b(std::istream& is)
{
	const auto grid = read_grid(is);
	const auto grid5 = multiply(grid, 5);
	auto solver = CavernSolver{grid5};
	const auto total_risk = solver.solve();
	return total_risk;
}
