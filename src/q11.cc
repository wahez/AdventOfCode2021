#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <cassert>


namespace
{


	struct Grid
	{
		int evolve()
		{
			rows.front().fill(0);
			for (auto& row: rows)
			{
				row.front() = 0;
				row.back() = 0;
			}
			rows.back().fill(0);
			for (auto row = 0u; row < rows.size(); ++row)
				for (auto col = 0u; col < rows[0].size(); ++col)
					increase(row, col);
			auto flashes = 0;
			for (auto& row: rows)
				for (auto& o: row)
					if (o > 9)
					{
						++flashes;
						o = 0;
					}
			return flashes;
		}

		friend std::istream& operator>>(std::istream& is, Grid& grid)
		{
			for (auto row = 1u; row < grid.rows.size()-1; ++row)
			{
				for (auto col = 1u; col < grid.rows[0].size()-1; ++col)
					grid.rows[row][col] = is.get()-'0';
				if (is.get() != '\n')
					throw std::runtime_error("Expected newline");
			}
			return is;
		}
	private:
		void increase(int row, int col)
		{
			assert(row >= 0 &&
				   col >= 0 &&
				   row < std::ssize(rows) &&
				   col < std::ssize(rows[0]));
			if (++rows[row][col] == 10)
			{
				increase(row-1, col-1);
				increase(row-1, col  );
				increase(row-1, col+1);
				increase(row  , col-1);
	//			increase(row  , col  );
				increase(row  , col+1);
				increase(row+1, col-1);
				increase(row+1, col  );
				increase(row+1, col+1);
			}
		}
		std::array<std::array<std::int8_t, 12>, 12> rows;
	};


}


int q11a(std::istream& is)
{
	auto grid = Grid{};
	is >> grid;
	auto flashes = 0;
	for (auto i = 0; i < 100; ++i)
		flashes += grid.evolve();
	return flashes;
}


int q11b(std::istream& is)
{
	auto grid = Grid{};
	is >> grid;
	for (auto i = 1; ; ++i)
		if (grid.evolve() == 100)
			return i;
}
