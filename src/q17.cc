#include <istream>
#include "util.h"


namespace
{


	struct Range
	{
		int min;
		int max;

		bool operator[](int v) const { return v >= min && v <= max; }
	};

	std::istream& operator>>(std::istream& is, Range& range)
	{
		is >> range.min >> Assert("..") >> range.max;
		return is;
	}


	struct Area
	{
		Range x;
		Range y;

		bool operator()(int xp, int yp) const { return x[xp] && y[yp]; }
	};

	std::istream& operator>>(std::istream& is, Area& area)
	{
		is >> Assert("target area: x=") >> area.x >> Assert(", y=") >> area.y;
		return is;
	}


	int y_position(int v0, int t)
	{
		return t*(2*v0-t+1)/2;
	}


	int x_position(int v0, int t)
	{
		if (t > v0)
			return v0*(v0+1)/2;
		else
			return t*(2*v0-t+1)/2;
	}


	int max_height(int vo)
	{
		return vo * (vo+1) / 2;
	}


}


int q17a(std::istream& is)
{
	auto target = Area{};
	is >> target;
	// brute force
	auto max_y = 0;
	for (auto vx = 1; vx < target.x.max; ++vx)
		for (auto vy = 1; vy < -target.y.min; ++vy)
			for (auto steps = 1;; ++steps)
			{
				const auto x = x_position(vx, steps);
				const auto y = y_position(vy, steps);
				if (x > target.x.max || y < target.y.min)
					break;
				else if (x < target.x.min || y > target.y.max)
					continue;
				else
					max_y = std::max(max_y, max_height(vy));
			}
	return max_y;
}
