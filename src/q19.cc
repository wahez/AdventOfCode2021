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
		int z;

		auto operator<=>(const Pos&) const = default;
	};


	Pos operator+(const Pos& lhs, const Pos& rhs) { return Pos{lhs.x+rhs.x, lhs.y+rhs.y, lhs.z+rhs.z}; }
	Pos operator-(const Pos& lhs, const Pos& rhs) { return Pos{lhs.x-rhs.x, lhs.y-rhs.y, lhs.z-rhs.z}; }

	std::istream& operator>>(std::istream& is, Pos& pos)
	{
		is >> pos.x >> Assert(',') >> pos.y >> Assert(',') >> pos.z;
		return is;
	}


	struct Rotation
	{
		bool x;
		bool y;
		bool z;
		bool swap_x_y;
		bool swap_x_z;
		bool swap_y_z;

		Pos rotate(Pos pos) const
		{
			if (x) pos.x = -pos.x;
			if (y) pos.y = -pos.y;
			if (z) pos.z = -pos.z;
			if (swap_x_y) std::tie(pos.x, pos.y) = std::make_tuple(pos.y, pos.x);
			if (swap_x_z) std::tie(pos.x, pos.z) = std::make_tuple(pos.z, pos.x);
			if (swap_y_z) std::tie(pos.y, pos.z) = std::make_tuple(pos.z, pos.y);
			return pos;
		}

		Pos unrotate(Pos pos) const
		{
			if (swap_y_z) std::tie(pos.y, pos.z) = std::make_tuple(pos.z, pos.y);
			if (swap_x_z) std::tie(pos.x, pos.z) = std::make_tuple(pos.z, pos.x);
			if (swap_x_y) std::tie(pos.x, pos.y) = std::make_tuple(pos.y, pos.x);
			if (z) pos.z = -pos.z;
			if (y) pos.y = -pos.y;
			if (x) pos.x = -pos.x;
			return pos;
		}

	};

	constexpr Rotation all_rotations[] = {
		{false, false, false, false, false, false},
		{false,  true,  true, false, false, false},
		{ true, false,  true, false, false, false},
		{ true,  true, false, false, false, false},

		{false, false,  true, false, false,  true},
		{false,  true, false, false, false,  true},
		{ true, false, false, false, false,  true},
		{ true,  true,  true, false, false,  true},

		{false, false,  true, false,  true, false},
		{false,  true, false, false,  true, false},
		{ true, false, false, false,  true, false},
		{ true,  true,  true, false,  true, false},

		{false, false,  true,  true, false, false},
		{false,  true, false,  true, false, false},
		{ true, false, false,  true, false, false},
		{ true,  true,  true,  true, false, false},

		{false, false, false, false,  true,  true},
		{false,  true,  true, false,  true,  true},
		{ true, false,  true, false,  true,  true},
		{ true,  true, false, false,  true,  true},

		{false, false, false,  true, false,  true},
		{false,  true,  true,  true, false,  true},
		{ true, false,  true,  true, false,  true},
		{ true,  true, false,  true, false,  true},
	};


	struct Box
	{
		explicit Box(Pos mn, Pos mx) :
			min{std::min(mn.x, mx.x), std::min(mn.y, mx.y), std::min(mn.z, mx.z)},
			max{std::max(mn.x, mx.x), std::max(mn.y, mx.y), std::max(mn.z, mx.z)}
		{}

		Pos min;
		Pos max;

		bool operator[](const Pos& p) const
		{
			return
				p.x >= min.x && p.x <= max.x &&
				p.y >= min.y && p.y <= max.y &&
				p.z >= min.z && p.z <= max.z;
		}
	};


	struct Beacons
	{
		int name;
		explicit Beacons(int i)
			: name(i)
		{
		}
		void translate(const Pos& pos)
		{
			std::ranges::for_each(positions, [&](auto& p) { p = p + pos; });
			std::ranges::for_each(known_area, [&](auto& k) { k = Box{k.min+pos, k.max+pos}; });
		}
		void rotate_all(const Rotation& rot)
		{
			std::ranges::for_each(positions, [&](auto& p) { p = rot.rotate(p); });
			std::ranges::for_each(known_area, [&](auto& k) { k = Box{rot.rotate(k.min), rot.rotate(k.max)}; });
		}

		std::optional<std::pair<Pos, Rotation>> match(const Beacons& other) const
		{
			for (const auto& pos1: positions)
			{
				for (const auto& rotation: all_rotations)
				{
					for (const auto& pos2: other.positions)
					{
						const auto translation = pos1 + rotation.rotate(Pos{0,0,0}-pos2);
						if (match(other, translation, rotation))
							return std::make_pair(translation, rotation);
					}
				}
			}
			return std::nullopt;
		}

		void add(Beacons other)
		{
			std::ranges::move(other.known_area, std::back_inserter(known_area));
			std::ranges::move(other.positions, std::back_inserter(positions));
			std::ranges::sort(positions);
			const auto to_remove = std::ranges::unique(positions);
			positions.erase(to_remove.begin(), to_remove.end());
		}

		auto size() const { return positions.size(); }

		Pos& emplace_back() { return positions.emplace_back(); }

	private:
		bool match(const Beacons& other, const Pos& translation, const Rotation& rotation) const
		{
			auto count = 0;
			for (const auto& position: other.positions)
			{
				const auto projected = translation+rotation.rotate(position);
				if (std::ranges::any_of(known_area, [&](const auto& area) { return area[projected]; }))
				{
					if (std::ranges::find(positions, projected) == positions.end())
						return false;
					else
						++count;
				}
			}
			for (const auto& position: positions)
			{
				const auto projected = rotation.unrotate(position-translation);
				if (std::ranges::any_of(other.known_area, [&](const auto& area) { return area[projected]; }))
				{
					if (std::ranges::find(other.positions, projected) == other.positions.end())
						return false;
				}
			}
			return count >= 2;
		}

		std::vector<Box> known_area{{Box{Pos{-1000,-1000,-1000}, Pos{1000,1000,1000}}}};
		std::vector<Pos> positions;
	};


	Beacons read_scanner(std::istream& is)
	{
		auto i = 0;
		is >> Assert("--- scanner ") >> i >> Assert(" ---\n");
		auto beacons = Beacons{i};
		while (is.peek() != EOF && is.peek() != '\n')
			is >> beacons.emplace_back() >> Assert('\n');
		is >> Assert('\n');
		return beacons;
	}


}


int q19a(std::istream& is)
{
	auto scanners = std::vector<Beacons>{};
	while (is.good())
		scanners.push_back(read_scanner(is));
	auto map = std::move(scanners.front());
	scanners.erase(scanners.begin());
	while (!scanners.empty())
	{
		for (auto it = scanners.begin(); it != scanners.end();)
		{
			if (const auto match_result = map.match(*it); match_result)
			{
				it->rotate_all(match_result->second);
				it->translate(match_result->first);
				map.add(std::move(*it));
				it = scanners.erase(it);
			}
			else
				++it;
		}
	}
	return map.size();
}
