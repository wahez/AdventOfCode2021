#include <cmath>
#include <istream>
#include "util.h"


namespace
{


	struct Range
	{
		int min;
		int max;
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
	};

	std::istream& operator>>(std::istream& is, Area& area)
	{
		is >> Assert("target area: x=") >> area.x >> Assert(", y=") >> area.y;
		return is;
	}


	int max_height(int vo)
	{
		return vo * (vo+1) / 2;
	}


	int min_speed_for_distance(int distance)
	{
		return (-1 + std::sqrt(1+8*distance))/2;
	}

	struct Probe
	{
		int vx;
		int vy;
		int x = 0;
		int y = 0;

		void move()
		{
			x += vx;
			y += vy;
			vx = std::max(0, vx-1);
			--vy;
		}
	};

}


int q17a(std::istream& is)
{
	auto target = Area{};
	is >> target;
	auto max_y = 0;
	for (auto vx = min_speed_for_distance(target.x.min); vx <= target.x.max; ++vx)
		for (auto vy = target.y.min; vy <= -target.y.min; ++vy)
			for (auto probe = Probe{vx, vy};; probe.move())
			{
				if (probe.x > target.x.max || probe.y < target.y.min)
					break;
				else if (probe.x < target.x.min || probe.y > target.y.max)
					continue;
				else
					max_y = std::max(max_y, max_height(vy));
			}
	return max_y;
}


int q17b(std::istream& is)
{
	auto target = Area{};
	is >> target;
	auto count = 0;
	for (auto vx = min_speed_for_distance(target.x.min); vx <= target.x.max; ++vx)
		for (auto vy = target.y.min; vy <= -target.y.min; ++vy)
			for (auto probe = Probe{vx, vy};; probe.move())
			{
				if (probe.x > target.x.max || probe.y < target.y.min)
					break;
				else if (probe.x < target.x.min || probe.y > target.y.max)
					continue;
				else
				{
					++count;
					break;
				}
			}
	return count;
}
