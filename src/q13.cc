#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace
{

	struct Fold
	{
		std::int16_t location;
		bool horizontally;
	};

	std::istream& operator>>(std::istream& is, Fold& fold)
	{
		auto fold_orientation = char{};
		is >> Assert("fold along ") >> fold_orientation >> Assert('=') >> fold.location >> Assert('\n');
		switch (fold_orientation)
		{
		case 'x': fold.horizontally = true; break;
		case 'y': fold.horizontally = false; break;
		default : if (is.good()) throw std::runtime_error("Expected \"x\" or \"y\"");
		}
		return is;
	}


	struct Dot
	{
		int x;
		int y;

		auto operator<=>(const Dot&) const = default;
	};

	struct Sheet
	{
		std::vector<Dot> dots;

		void apply(const Fold& fold)
		{
			if (fold.horizontally)
			{
				for (auto& dot: dots)
					if (dot.x > fold.location)
						dot.x = 2 * fold.location - dot.x;
			}
			else
			{
				for (auto& dot: dots)
					if (dot.y > fold.location)
						dot.y = 2 * fold.location - dot.y;
			}
			std::ranges::sort(dots);
			const auto to_remove = std::ranges::unique(dots);
			dots.erase(to_remove.begin(), to_remove.end());
		}

		bool contains(const Dot& dot) const
		{
			return std::ranges::find(dots, dot) != dots.end();
		}
	};

	std::istream& operator>>(std::istream& is, Sheet& sheet)
	{
		sheet.dots.clear();
		while (is.good() && is.peek() != '\n')
		{
			auto& dot = sheet.dots.emplace_back();
			is >> dot.x >> Assert{','} >> dot.y >> Assert{'\n'};
		}
		return is;
	}

	std::ostream& operator<<(std::ostream& os, const Sheet& sheet)
	{
		const auto max_x = std::ranges::max(sheet.dots, std::less{}, &Dot::x).x;
		const auto max_y = std::ranges::max(sheet.dots, std::less{}, &Dot::y).y;
		for (auto y = 0; y <= max_y; ++y)
		{
			for (auto x = 0; x <= max_x; ++x)
				if (sheet.contains(Dot{x,y}))
					os << '#';
				else
					os << ' ';
			os << '\n';
		}
		return os;
	}

}


int q13a(std::istream& is)
{
	auto sheet = Sheet{};
	auto fold = Fold{};
	is >> sheet >> Assert('\n') >> fold;
	sheet.apply(fold);
	return sheet.dots.size();
}


std::string q13b(std::istream& is)
{
	auto sheet = Sheet{};
	is >> sheet >> Assert('\n');
	for (const auto& fold: std::ranges::istream_view<Fold>(is))
		sheet.apply(fold);
	std::ostringstream os;
	os << sheet;
	return os.str();
}
