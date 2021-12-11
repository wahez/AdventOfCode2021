#include <array>
#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace {


	struct HeightMap
	{
		using Height = std::int8_t;
		using Pos = int;

		Height operator[](const Pos& pos) const { return height[pos]; }

		Pos get_top_left() const { return line_size+1; }
		Pos get_bottom_right() const { return height.size() - line_size - 1; }

		int fill_basin(Pos pos)
		{
			if (height[pos] == 9)
				return 0;
			else
			{
				height[pos] = 9;
				return 1 +
					fill_basin(pos+1) +
					fill_basin(pos-1) +
					fill_basin(pos+line_size) +
					fill_basin(pos-line_size);
			}
		}

		bool is_lower_than_neighbours(const Pos& pos) const
		{
			auto lower = [&](auto p) { return (*this)[pos] < (*this)[p]; };
			return lower(pos+1) &&
			       lower(pos-1) &&
			       lower(pos+line_size) &&
			       lower(pos-line_size);
		};

		static HeightMap read(std::istream& is)
		{
			std::noskipws(is);
			auto map = HeightMap{};
			auto lines = 0;
			for (; is.good(); ++lines)
			{
				map.height.push_back(9);
				auto line_view = std::ranges::istream_view<char>(is) | std::views::take_while([](char c) { return c != '\n'; });
				for (const char c: line_view)
				{
					const auto height = c - '0';
					if (height < 0 || height > 9)
						throw std::runtime_error("Invalid height");
					map.height.push_back(height);
				}
				map.height.push_back(9);
			}
			--lines;
			map.height.erase(map.height.end()-2, map.height.end());
			if (map.height.size() % lines != 0)
				throw std::runtime_error("lines have uneven length");
			map.line_size = map.height.size() / lines;
			map.height.insert(map.height.begin(), map.line_size, 9);
			map.height.insert(map.height.end(), map.line_size, 9);
			return map;
		}

		int risk_level(Pos pos) const { return height[pos] + 1; }

	private:
		std::vector<Height> height;
		int line_size;
	};


}


int q09a(std::istream& is)
{
	const auto map = HeightMap::read(is);
	auto low_points = std::vector<HeightMap::Pos>{};
	for (auto pos = map.get_top_left(); pos < map.get_bottom_right(); ++pos)
		if (map.is_lower_than_neighbours(pos))
			low_points.push_back(pos);
	return accumulate(low_points | std::views::transform([&](auto&& p) { return map.risk_level(p); }), 0);
}


int q09b(std::istream& is)
{
	auto map = HeightMap::read(is);
	std::vector<int> basins;
	for (auto pos = map.get_top_left(); pos < map.get_bottom_right(); ++pos)
	{
		const auto num_filled = map.fill_basin(pos);
		if (num_filled > 0)
			basins.push_back(num_filled);
	}
	std::ranges::sort(basins, std::greater<>{});
	assert(basins.size() > 2);
	return basins.at(0) * basins.at(1) * basins.at(2);
}
