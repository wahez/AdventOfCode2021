#include <algorithm>
#include <istream>
#include <map>
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

		friend std::ostream& operator<<(std::ostream& os, const Pos& pos)
		{
			os << pos.x << ',' << pos.y << ',' << pos.z;
			return os;
		}

		int manhattan() const
		{
			return std::abs(x) + std::abs(y) + std::abs(z);
		}
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
		std::array<std::array<std::int8_t, 3>, 3> m;

		Pos rotate(Pos pos) const
		{
			return Pos{
				m[0][0] * pos.x + m[0][1] * pos.y + m[0][2] * pos.z,
				m[1][0] * pos.x + m[1][1] * pos.y + m[1][2] * pos.z,
				m[2][0] * pos.x + m[2][1] * pos.y + m[2][2] * pos.z};
		}

		Pos unrotate(Pos pos) const
		{
			return Pos{
				m[0][0] * pos.x + m[1][0] * pos.y + m[2][0] * pos.z,
				m[0][1] * pos.x + m[1][1] * pos.y + m[2][1] * pos.z,
				m[0][2] * pos.x + m[1][2] * pos.y + m[2][2] * pos.z};
		}

		friend std::ostream& operator<<(std::ostream& os, const Rotation& rot)
		{
			for (auto r: rot.m)
			{
				for (auto c: r)
					os << NotChar(c) << " ";
				os << '\n';
			}
			return os;
		}
	};


	constexpr Rotation all_rotations[] = {
		{{{{{ 1, 0, 0}}, {{ 0, 1, 0}}, {{ 0, 0, 1}}}}},
		{{{{{ 1, 0, 0}}, {{ 0,-1, 0}}, {{ 0, 0,-1}}}}},
		{{{{{ 1, 0, 0}}, {{ 0, 0, 1}}, {{ 0,-1, 0}}}}},
		{{{{{ 1, 0, 0}}, {{ 0, 0,-1}}, {{ 0, 1, 0}}}}},
		{{{{{-1, 0, 0}}, {{ 0,-1, 0}}, {{ 0, 0, 1}}}}},
		{{{{{-1, 0, 0}}, {{ 0, 1, 0}}, {{ 0, 0,-1}}}}},
		{{{{{-1, 0, 0}}, {{ 0, 0,-1}}, {{ 0,-1, 0}}}}},
		{{{{{-1, 0, 0}}, {{ 0, 0, 1}}, {{ 0, 1, 0}}}}},
		{{{{{ 0, 1, 0}}, {{-1, 0, 0}}, {{ 0, 0, 1}}}}},
		{{{{{ 0, 1, 0}}, {{ 1, 0, 0}}, {{ 0, 0,-1}}}}},
		{{{{{ 0, 1, 0}}, {{ 0, 0,-1}}, {{-1, 0, 0}}}}},
		{{{{{ 0, 1, 0}}, {{ 0, 0, 1}}, {{ 1, 0, 0}}}}},
		{{{{{ 0,-1, 0}}, {{ 1, 0, 0}}, {{ 0, 0, 1}}}}},
		{{{{{ 0,-1, 0}}, {{-1, 0, 0}}, {{ 0, 0,-1}}}}},
		{{{{{ 0,-1, 0}}, {{ 0, 0, 1}}, {{-1, 0, 0}}}}},
		{{{{{ 0,-1, 0}}, {{ 0, 0,-1}}, {{ 1, 0, 0}}}}},
		{{{{{ 0, 0, 1}}, {{-1, 0, 0}}, {{ 0,-1, 0}}}}},
		{{{{{ 0, 0, 1}}, {{ 1, 0, 0}}, {{ 0, 1, 0}}}}},
		{{{{{ 0, 0, 1}}, {{ 0,-1, 0}}, {{ 1, 0, 0}}}}},
		{{{{{ 0, 0, 1}}, {{ 0, 1, 0}}, {{-1, 0, 0}}}}},
		{{{{{ 0, 0,-1}}, {{ 1, 0, 0}}, {{ 0,-1, 0}}}}},
		{{{{{ 0, 0,-1}}, {{-1, 0, 0}}, {{ 0, 1, 0}}}}},
		{{{{{ 0, 0,-1}}, {{ 0, 1, 0}}, {{ 1, 0, 0}}}}},
		{{{{{ 0, 0,-1}}, {{ 0,-1, 0}}, {{-1, 0, 0}}}}},
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
			std::vector<Pos> translations;
			for (const auto& rotation: all_rotations)
			{
				translations.resize(0);
				for (const auto& pos2: other.positions)
				{
					const auto rotated = rotation.rotate(Pos{0,0,0}-pos2);
					for (const auto& pos1: positions)
						translations.push_back(pos1+rotated);
				}
				std::ranges::sort(translations);
				auto first = translations.begin();
				auto last = first;
				while (last != translations.end())
				{
					last = std::find_if(first, translations.end(), [&](const auto& value) { return value != *first; });
					if (std::distance(first, last) >= 12)
						return std::make_pair(*first, rotation);
					first = last;
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


int q19b(std::istream& is)
{
	auto scanners = std::vector<Beacons>{};
	while (is.good())
		scanners.push_back(read_scanner(is));
	auto map = std::move(scanners.front());
	scanners.erase(scanners.begin());
	auto positions = std::vector{Pos{0,0,0}};
	while (!scanners.empty())
	{
		for (auto it = scanners.begin(); it != scanners.end();)
		{
			if (const auto match_result = map.match(*it); match_result)
			{
				positions.push_back(match_result->first);
				it->rotate_all(match_result->second);
				it->translate(match_result->first);
				map.add(std::move(*it));
				it = scanners.erase(it);
			}
			else
				++it;
		}
	}
	auto max = 0;
	for (auto i1 = 0; i1 < std::ssize(positions); ++i1)
		for (auto i2 = 0; i2 < std::ssize(positions); ++i2)
			max = std::max(max, (positions[i1] - positions[i2]).manhattan());
	return max;
}
