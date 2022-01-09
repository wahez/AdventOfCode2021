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


	struct Bitset
	{
		using value_type = bool;

		explicit Bitset(int size) : bits(std::max<int>(0, (size-1)/num_bits+1)), sz(size) {}

		int size() const { return sz; }

		void push_back(bool v)
		{
			++sz;
			bits.resize((sz-1)/num_bits+1, 0);
			set(sz-1, v);
		}

		void set(int i, bool v)
		{
			const auto idx = i / num_bits;
			const auto b = i % num_bits;
			const auto mask = ~(std::uintmax_t{1} << std::uintmax_t{b});
			bits[idx] = (bits[idx] & mask) | (std::uintmax_t{v} << std::uintmax_t{b});
		}

		bool operator[](int i) const
		{
			const auto idx = i / num_bits;
			const auto b = i % num_bits;
			const auto mask = std::uintmax_t{1} << std::uintmax_t{b};
			return bits[idx] & mask;
		}

		int count() const
		{
			return accumulate(bits | std::views::transform([](auto&& e) { return std::popcount(e); }), 0);
		}

	private:
		std::vector<std::uintmax_t> bits;
		int sz;
		static constexpr auto num_bits = 8*sizeof(std::uintmax_t);
	};


	class Image
	{
	public:
		explicit Image(int szx = 0, int szy = 0) :
			pixels(szx * szy),
			line_width(szx),
			num_lines(szy)
		{}

		int pixels_lit() const
		{
			return pixels.count();
		}

		void set_pixel(int x, int y, bool value)
		{
			pixels.set(x + y * line_width, value);
		}

		auto get_pixel(int x, int y) const
		{
			return pixels[x + y * line_width];
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
		auto size_y() const { return num_lines; }

		template<bool safe=true>
		FixedSquareImage<3> get_pixel_with_neighbours(int x, int y) const
		{
			auto bits = FixedSquareImage<3>{};
			auto bit = 9;
			for (auto ys = y-1; ys < y+2; ++ys)
				for (auto xs = x-1; xs < x+2; ++xs)
				{
					if constexpr (!safe)
						bits[--bit] = get_pixel(xs, ys);
					else
					{
						if (xs < 0 || ys < 0 || xs >= size_x() || ys >= size_y())
							bits[--bit] = background;
						else
							bits[--bit] = get_pixel(xs, ys);
					}
				}
			return bits;
		}

		friend std::istream& operator>>(std::istream& is, Image& image)
		{
			auto num_lines = 0;
			std::noskipws(is);
			auto bit_line = std::ranges::istream_view<char>(is)
					| std::views::take_while([](char c) { return c != '\n'; })
					| std::views::transform([](char c) { return c == '#'; });
			while (is.good())
			{
				std::ranges::copy(bit_line, std::back_inserter(image.pixels));
				++num_lines;
			}
			--num_lines;
			assert(std::ssize(image.pixels) % num_lines == 0);
			image.line_width = image.pixels.size() / num_lines;
			image.num_lines = num_lines;
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
		Bitset pixels;
		int line_width;
		int num_lines;
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
			auto result = Image{image.size_x()+2, image.size_y()+2};
			if (image.get_background())
				result.set_background(bits[0]);
			else
				result.set_background(bits[511]);
			for (auto x: {0, 1, result.size_x()-2, result.size_x()-1})
				for (auto y = 0; y < result.size_y(); ++y)
				{
					const auto bits = image.get_pixel_with_neighbours(x-1, y-1);
					result.set_pixel(x, y, enhance_pixel(bits));
				}
			for (auto y: {0, 1, result.size_y()-2, result.size_y()-1})
				for (auto x = 0; x < result.size_x(); ++x)
				{
					const auto bits = image.get_pixel_with_neighbours(x-1, y-1);
					result.set_pixel(x, y, enhance_pixel(bits));
				}
			for (auto y = 2; y < result.size_y()-2; ++y)
				for (auto x = 2; x < result.size_x()-2; ++x)
				{
					const auto bits = image.get_pixel_with_neighbours<false>(x-1, y-1);
					result.set_pixel(x, y, enhance_pixel(bits));
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
