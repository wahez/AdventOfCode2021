#include <algorithm>
#include <bitset>
#include <cassert>
#include <istream>
#include <ranges>
#include <queue>
#include "util.h"


namespace
{


	template<int n_pixels>
	using FixedSquareImage = std::bitset<n_pixels*n_pixels>;


	struct Bitset
	{
		using value_type = bool;

		explicit Bitset(int size, bool v) : bits((size-1)/num_bits+1, v ? ~std::uintmax_t{0} : 0), sz(size) {}

		int size() const { return sz; }

		void resize(int s)
		{
			bits.resize((s-1)/num_bits+1);
			sz = s;
		}

		Bitset& push_back(bool v)
		{
			const auto i = sz;
			resize(sz+1);
			set(i, v);
			return *this;
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
			auto c = 0;
			for (auto i = 0; i < std::ssize(bits)-1; ++i)
				c += std::popcount(bits[i]);
			return c + std::popcount(bits.back() << (num_bits-(sz%num_bits)));
		}

		Bitset& operator<<=(int i)
		{
			const auto to_add = i / num_bits;
			const auto to_shift = i % num_bits;
			resize(sz + to_shift);
			if (i % num_bits != 0)
				for (auto i = std::ssize(bits)-1; i > 1; --i)
				{
					const auto b = bits[i-1];
					bits[i] = (bits[i] << to_shift) | (b >> (num_bits-to_shift));
					bits[i-1] = b << to_shift;
				}
			bits.push_front(to_add);
			resize(sz + to_add * num_bits);
			return *this;
		}

		[[nodiscard]]
		Bitset concat(const Bitset& other) const
		{
			auto result = *this;
			result <<= other.size();
			result |= other;
			return result;
		}

		Bitset& operator|=(Bitset other)
		{
			do_op(std::move(other), [](auto&& l, auto&& r) { return l |= r; });
			return *this;
		}

		std::intmax_t operator&(std::intmax_t mask) const
		{
			return bits[0] & mask;
		}

		Bitset& operator>>=(int i)
		{
			const auto to_erase = i / num_bits;
			const auto to_shift = i % num_bits;
			bits.erase(bits.begin(), bits.begin() + to_erase);
			if (!bits.empty())
				bits[0] >>= to_shift;
			for (auto i = 1; i < std::ssize(bits); ++i)
			{
				const auto b = bits[i];
				bits[i-1] |= b << (num_bits-to_shift);
				bits[i] = b >> to_shift;
			}
			resize(sz-i);
			return *this;
		}

		Bitset operator>>(int i) const
		{
			auto result = *this;
			return result >>= i;
		}

	private:
		void do_op(Bitset other, auto&& func)
		{
			const auto new_size = std::max(size(), other.size());
			resize(new_size);
			other.resize(new_size);
			for (auto i = 0; i < std::ssize(bits); ++i)
				func(bits[i], other.bits[i]);
		}

		std::deque<std::uintmax_t> bits;
		int sz;
		static constexpr auto num_bits = 8*sizeof(std::uintmax_t);
	};


	class Image
	{
	public:
		explicit Image(int szx = 0, int szy = 0) :
			pixels(szx * szy, false),
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

		auto get_pixel_row(int y) const
		{
			auto row = pixels;
			row <<= y * line_width;
			row.resize(line_width);
			return row;
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
			const auto background = image.get_background();
			if (background)
				result.set_background(bits[0]);
			else
				result.set_background(bits[511]);
			auto rows = std::array<Bitset, 3>{{
				Bitset{result.size_x(), background},
				Bitset{result.size_x(), background},
				Bitset{result.size_x(), background}}};
			// TODO lambda
			for (auto y = 0; y < image.size_y(); ++y)
			{
				rows[0] = rows[1];
				rows[1] = rows[2];
				rows[2] = Bitset{2, background}.concat(image.get_pixel_row(y)).concat(Bitset{2, background});
				for (auto x = 0; x < image.size_x(); ++x)
				{
					const auto bits = ((rows[0] >> (x-6)) & 0b111000000)
					                | ((rows[1] >> (x-3)) & 0b000111000)
					                | ((rows[2] >>  x   ) & 0b000000111);
					result.set_pixel(x, y, enhance_pixel(bits));
				}
			}
			for (auto y = image.size_y(); y < result.size_y(); ++y)
			{
				rows[0] = rows[1];
				rows[1] = rows[2];
				rows[2] = Bitset{result.size_x(), background};
				for (auto x = 0; x < image.size_x(); ++x)
				{
					const auto bits = ((rows[0] >> (x-6)) & 0b111000000)
					                | ((rows[1] >> (x-3)) & 0b000111000)
					                | ((rows[2] >>  x   ) & 0b000000111);
					result.set_pixel(x, y, enhance_pixel(bits));
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
