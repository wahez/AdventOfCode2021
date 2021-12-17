#include <algorithm>
#include <istream>
#include <ranges>
#include <string>
#include <vector>


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
		void add(const Tunnel& tunnel)
		{
			auto find_index = [&](const auto& cave) -> int
			{
				if (const auto it = std::ranges::find(caves, cave, &Cave::name); it != caves.end())
					return std::distance(caves.begin(), it);
				else
				{
					caves.emplace_back(cave);
					return caves.size()-1;
				}
			};
			const auto i1 = find_index(tunnel.cave1);
			const auto i2 = find_index(tunnel.cave2);
			caves[i1].connections.push_back(i2);
			caves[i2].connections.push_back(i1);
		}

		int find_paths(bool allow_duplicate)
		{
			num_paths = 0;
			const auto it = std::ranges::find(
				caves,
				"start",
				&Cave::name);
			count_paths_from(it->connections, allow_duplicate);
			return num_paths;
		}

	private:
		enum Type
		{
			END = 1,
			BIG = 2,
			SMALL = 3,
			SMALL_VISITED = 4,
			SMALL_DISABLED = 0,
		};
		static Type from_name(const std::string& name)
		{
			if (name == "start") return SMALL_DISABLED;
			else if (name == "end") return END;
			else if (std::isupper(name[0]) != 0) return BIG;
			else return SMALL;
		}
		struct Cave
		{
			explicit Cave(std::string nm) :
				name(std::move(nm)),
				type(from_name(name))
			{}
			std::string name;
			std::vector<std::int8_t> connections;
			Type type;
		};

		void count_paths_from(Cave& cave, const bool allow_duplicate)
		{
			switch (cave.type)
			{
			case SMALL_DISABLED:
				break;
			case END:
				++num_paths;
				break;
			case BIG:
				count_paths_from(cave.connections, allow_duplicate);
				break;
			case SMALL:
				cave.type = SMALL_VISITED;
				count_paths_from(cave.connections, allow_duplicate);
				cave.type = SMALL;
				break;
			case SMALL_VISITED:
				if (allow_duplicate)
				{
					cave.type = SMALL_DISABLED;
					count_paths_from(cave.connections, false);
					cave.type = SMALL_VISITED;
				}
				break;
			}
		}
		void count_paths_from(const std::vector<std::int8_t>& next_cave_indices, const bool allow_duplicate)
		{
			for (const auto& next_cave_index: next_cave_indices)
				count_paths_from(caves[next_cave_index], allow_duplicate);
		}
		std::vector<Cave> caves;
		int num_paths;
	};


}


int q12a(std::istream& is)
{
	auto solver = CaveSolver();
	for (const auto& tunnel: std::ranges::istream_view<Tunnel>(is))
		solver.add(tunnel);
	return solver.find_paths(false);
}


int q12b(std::istream& is)
{
	auto solver = CaveSolver();
	for (const auto& tunnel: std::ranges::istream_view<Tunnel>(is))
		solver.add(tunnel);
	return solver.find_paths(true);
}
