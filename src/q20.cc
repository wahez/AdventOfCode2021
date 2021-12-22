#include <algorithm>
#include <bitset>
#include <cassert>
#include <istream>
#include <ranges>
#include <vector>
#include "util.h"


namespace
{


	template<int n_pixels>
	using FixedSquareImage = std::bitset<n_pixels*n_pixels>;


	class Image
	{
	public:
		explicit Image(int szx = 0, int szy = 0) :
			pixels(szx * szy, false),
			line_width(szx)
		{}

		int pixels_lit() const
		{
			return std::ranges::count(pixels, true);
		}

		void set_pixel(int x, int y, bool value)
		{
			pixels[x + y * line_width] = value;
		}

		void set_background(bool value)
		{
			background = value;
		}

		bool get_background() const
		{
			return background;
		}

		auto size_x() const { return line_width; }
		auto size_y() const { return std::ssize(pixels) / line_width; }

		auto get_pixel_with_neighbours(int xm, int ym) const
		{
			auto result = FixedSquareImage<3>{};
			for (auto y = ym-1; y < ym+2; ++y)
				for (auto x = xm-1; x < xm+2; ++x)
				{
					if (x < 0 || y < 0 || x >= size_x() || y >= size_y())
						result[4 -x+xm -(y-ym)*3] = background;
					else
						result[4 -x+xm -(y-ym)*3] = pixels[x+y*line_width];
				}
			return result;
		}

		friend std::istream& operator>>(std::istream& is, Image& image)
		{
			auto num_lines = 0;
			while (is.good())
			{
				switch (is.get())
				{
				case '.':
					image.pixels.push_back(false);
					break;
				case '#':
					image.pixels.push_back(true);
					break;
				case EOF:
					break;
				case '\n':
					++num_lines;
					image.line_width = std::ssize(image.pixels) / num_lines;
					if (is.peek() == '\n')
					{
						assert(std::ssize(image.pixels) % num_lines == 0);
						return is;
					}
					break;
				default:
					throw std::runtime_error("Could not parse image");
				}
			}
			return is;
		}

		friend std::ostream& operator<<(std::ostream& os, const Image& image)
		{
			for (auto y = 0; y < image.size_y(); ++y)
			{
				for (auto x = 0; x < image.size_x(); ++x)
					os << (image.pixels[x+y*image.line_width] ? '#' : ' ');
				os << '\n';
			}
			return os;
		}
	private:
		std::vector<bool> pixels;
		int line_width;
		bool background=false;
	};


	struct PixelEnhancer
	{
		bool enhance_pixel(const FixedSquareImage<3>& pixel_with_neighbours) const
		{
			return bits[511-pixel_with_neighbours.to_ulong()];
		}

		Image enhance(const Image& image) const
		{
			auto result = Image{image.size_x()+2, static_cast<int>(image.size_y()+2)};
			if (image.get_background())
				result.set_background(bits[0]);
			else
				result.set_background(bits[511]);
			for (auto x = 0; x < result.size_x(); ++x)
			{
				for (auto y = 0; y < result.size_y(); ++y)
				{
					const auto pixel_with_neighbours = image.get_pixel_with_neighbours(x-1, y-1);
					result.set_pixel(x, y, enhance_pixel(pixel_with_neighbours));
				}
			}
			return result;
		}

		std::bitset<512> bits;
	};


}


int q20a(std::istream& is)
{
	const auto line = read<std::string>(is);
	const auto pixel_enhancer = PixelEnhancer{std::bitset<512>(line, 0, 512, '.', '#')};
	is >> Assert("\n\n");
	auto image = read<Image>(is);
	image = pixel_enhancer.enhance(image);
	image = pixel_enhancer.enhance(image);
	return image.pixels_lit();
}


int q20b(std::istream& is)
{
	const auto line = read<std::string>(is);
	const auto pixel_enhancer = PixelEnhancer{std::bitset<512>(line, 0, 512, '.', '#')};
	is >> Assert("\n\n");
	auto image = read<Image>(is);
	for (auto i = 0; i < 50; ++i)
		image = pixel_enhancer.enhance(image);
	return image.pixels_lit();
}
