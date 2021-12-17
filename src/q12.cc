#include <algorithm>
#include <array>
#include <iostream>
#include <ranges>
#include <set>
#include <vector>
#include <cassert>
#include "util.h"


namespace
{


	struct Tunnel
	{
		std::string cave1;
		std::string cave2;
	};

	std::istream& operator>>(std::istream& is, Tunnel& tunnel)
	{
		std::getline(is, tunnel.cave1, '-');
		std::getline(is, tunnel.cave2, '\n');
		return is;
	}


	struct CaveSolver
	{
		explicit CaveSolver(const auto& tunnels)
		{
			std::set<std::string> caveset;
			for (const auto& [cave1, cave2]: tunnels)
			{
				caveset.insert(cave1);
				caveset.insert(cave2);
			}
			for (const auto& cave: caveset)
				caves.push_back(Cave{{}, std::isupper(cave[0])!=0, cave == "start", cave == "end"});
			auto find_index = [&](const auto& cave) -> int
			{
				const auto it = caveset.find(cave);
				return std::distance(caveset.begin(), it);
			};
			for (const auto& [cave1, cave2]: tunnels)
			{
				const auto i1 = find_index(cave1);
				const auto i2 = find_index(cave2);
				caves[i1].connections.push_back(i2);
				caves[i2].connections.push_back(i1);
			}
		}

		int find_paths()
		{
			const auto it = std::ranges::find(
				caves,
				true,
				&Cave::start);
			return find_paths(std::distance(caves.begin(), it));
		}

	private:
		int find_paths(int i)
		{
			auto& cave = caves[i];
			if (cave.end)
				return 1;
			if (cave.connections.empty())
				return 0;
			auto find_paths_for = [&](const auto& caves)
			{
				auto num_paths = 0;
				for (const auto& next_cave: caves)
					num_paths += find_paths(next_cave);
				return num_paths;
			};
			if (cave.big)
				return find_paths_for(cave.connections);
			else
			{
				auto next_caves = std::move(cave.connections);
				cave.connections.clear();
				const auto num_paths = find_paths_for(next_caves);
				cave.connections = std::move(next_caves);
				return num_paths;
			}
			return 0;
		}
		struct Cave
		{
			std::vector<int> connections;
			bool big = false;
			bool start = false;
			bool end = false;
		};
		std::vector<Cave> caves;
	};


}


int q12a(std::istream& is)
{
	auto tunnels = std::vector<Tunnel>{};
	std::ranges::move(std::ranges::istream_view<Tunnel>(is), std::back_inserter(tunnels));
	auto solver = CaveSolver(tunnels);
	return solver.find_paths();
}
