#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace {


	struct Pos
	{
		int x;
		int y;

		Pos operator+(const Pos& rhs) const { return Pos{x+rhs.x, y+rhs.y}; }
		auto operator<=>(const Pos&) const = default;
	};


	struct HeightMap
	{
		HeightMap(std::vector<int> heights, int num_lines) :
			height(std::move(heights)),
			maxx(height.size() / num_lines)
		{}

		int operator[](const Pos& pos) const
		{
			return height.at(pos.x + pos.y * maxx);
		}

		int max_x() const { return maxx; }
		int max_y() const { return height.size() / maxx; }

	private:
		std::vector<int> height;
		int maxx;
	};


	HeightMap read_map(std::istream& is)
	{
		auto numbers = std::vector<int>{};
		auto lines = 0;
		for (auto c = is.get(); is.good(); c = is.get())
		{
			if (c == '\n')
				++lines;
			else
				numbers.push_back(c - '0');
		}
		if (numbers.size() % lines != 0)
			throw std::runtime_error("lines have uneven length");
		return HeightMap(numbers, lines);
	}


	auto find_low_points(const HeightMap& map)
	{
		static constexpr auto xu = Pos{ 1,  0};
		static constexpr auto xd = Pos{-1,  0};
		static constexpr auto yd = Pos{ 0, -1};
		static constexpr auto yu = Pos{ 0,  1};
		auto lowest_points = std::vector<Pos>{};
		auto is_lowest = [&](const auto& pos, const auto&... neighbours)
		{
			return ((map[pos] < map[pos+neighbours]) && ... && true);
		};
		auto pos = Pos{0, 0};
		if (is_lowest(pos, xu, yu))
			lowest_points.push_back(pos);
		for (pos.x = 1; pos.x < map.max_x()-1; ++pos.x)
			if (is_lowest(pos, xu, xd, yu))
				lowest_points.push_back(pos);
		if (is_lowest(pos, xd, yu))
			lowest_points.push_back(pos);
		for (pos.y = 1; pos.y < map.max_y()-1; ++pos.y)
		{
			pos.x = 0;
			if (is_lowest(pos, xu, yu, yd))
				lowest_points.push_back(pos);
			for (pos.x = 1; pos.x < map.max_x()-1; ++pos.x)
				if (is_lowest(pos, xu, xd, yu, yd))
					lowest_points.push_back(pos);
			if (is_lowest(pos, xd, yu, yd))
				lowest_points.push_back(pos);
		}
		pos.x = 0;
		if (is_lowest(pos, xu, yd))
			lowest_points.push_back(pos);
		for (pos.x = 1; pos.x < map.max_x()-1; ++pos.x)
			if (is_lowest(pos, xu, xd, yd))
				lowest_points.push_back(pos);
		if (is_lowest(pos, xd, yd))
			lowest_points.push_back(pos);
		return lowest_points;
	}


}


int q09a(std::istream& is)
{
	const auto map = read_map(is);
	const auto low_points = find_low_points(map);
	return accumulate(low_points | std::views::transform([&](auto&& p) { return map[p] + 1; }), 0);
}
