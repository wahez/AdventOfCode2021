#include <algorithm>
#include <istream>
#include <optional>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace
{

	class Bitset
	{
	public:
		constexpr explicit Bitset() : bits(0), bits_used(0) {}
		constexpr explicit Bitset(std::uint8_t b) : bits(b), bits_used(4) {}

		constexpr std::int8_t size() const { return bits_used; }

		constexpr std::uint8_t extract(int num_bits)
		{
			assert(num_bits <= bits_used);
			const auto shifted = bits >> (bits_used-num_bits);
			const auto mask = (1<<num_bits)-1;
			bits_used -= num_bits;
			return shifted & mask;
		}

	private:
		std::uint8_t bits = 0;
		std::int8_t bits_used = 0;
	};


	class BitReader
	{
	public:
		explicit BitReader(std::istream& i) : is(i) {}

		auto get_bits_consumed() const { return bits_consumed; }

		template<typename T>
		void consume(int num_bits, T& target)
		{
			assert(sizeof(T)*8 >= static_cast<std::size_t>(num_bits));
			bits_consumed += num_bits;
			auto value = 0ull;
			auto read_bits = [&](const auto bits_to_read)
			{
				value = (value << bits_to_read) | cached_bits.extract(bits_to_read);
				num_bits -= bits_to_read;
			};
			read_bits(std::min<int>(num_bits, cached_bits.size()));
			while (num_bits >= 4)
			{
				ensure_bits_present();
				read_bits(4);
			}
			ensure_bits_present();
			read_bits(num_bits);
			target = static_cast<T>(value);
		}

	private:
		std::istream& is;
		Bitset cached_bits;
		int bits_consumed = 0;

		void ensure_bits_present()
		{
			if (cached_bits.size() == 0)
			{
				const auto c = is.get();
				if (!is.good() || c == EOF)
					throw std::runtime_error("EOF");
				else if (c > '9')
					cached_bits = Bitset(c - 'A' + 10);
				else
					cached_bits = Bitset(c - '0');
			}
		}
	};


	enum class TypeID : std::int8_t
	{
		LITERAL = 4,
	};


	enum class LengthTypeID : bool
	{
		NUM_BITS = 0,
		NUM_PACKETS = 1,
	};


	struct PacketHeader
	{
		std::int8_t version;
		TypeID type_id;
		LengthTypeID length_type_id;
	};


	struct Packet
	{
		PacketHeader header;
		std::optional<std::int64_t> number;
		std::vector<Packet> sub_packets;

		void print(std::ostream& os, int indent = 0) const
		{
			os << std::string(indent, ' ') << NotChar(header.version) << "|" << static_cast<int>(header.type_id) << "|" << static_cast<int>(header.length_type_id) << '\n';
			os << std::string(indent, ' ') << sub_packets.size() << '\n';
			for (const auto& sp: sub_packets)
				sp.print(os, indent+2);
		}
	};

	auto read_literal(BitReader& bit_reader)
	{
		auto value = 0ll;
		auto continue_bit = 0;
		do
		{
			bit_reader.consume(1, continue_bit);
			auto next_bits = 0;
			bit_reader.consume(4, next_bits);
			value <<= 4;
			value |= next_bits;
		}
		while (continue_bit);
		return value;
	}

	auto read(BitReader& bit_reader, auto&& func)
	{
		using ResultType = decltype(func(PacketHeader{}, 0));
		auto header = PacketHeader{};
		bit_reader.consume(3, header.version);
		bit_reader.consume(3, header.type_id);
		if (header.type_id == TypeID::LITERAL)
			return func(header, read_literal(bit_reader));
		else
		{
			auto sub_results = std::vector<ResultType>{};
			bit_reader.consume(1, header.length_type_id);
			switch (header.length_type_id)
			{
			case LengthTypeID::NUM_BITS:
			{
				auto num_bits = 0;
				bit_reader.consume(15, num_bits);
				const auto current_bits_consumed = bit_reader.get_bits_consumed();
				while (bit_reader.get_bits_consumed() < current_bits_consumed + num_bits)
					sub_results.push_back(read(bit_reader, func));
				break;
			}
			case LengthTypeID::NUM_PACKETS:
			{
				auto num_sub_packets = 0;
				bit_reader.consume(11, num_sub_packets);
				for (auto i = 0; i < num_sub_packets; ++i)
					sub_results.push_back(read(bit_reader, func));
				break;
			}
			}
			return func(header, std::move(sub_results));
		}
	}


	int sum_versions(const Packet& packet)
	{
		return accumulate(packet.sub_packets | std::views::transform(sum_versions), static_cast<int>(packet.header.version));
	}


	std::istream& operator>>(std::istream& is, Packet& packet)
	{
		auto bit_reader = BitReader(is);
		if (is.good() && is.peek() != '\n' && is.peek() != EOF)
		{
			packet = read(bit_reader, [](const PacketHeader& header, auto sub) -> Packet
			{
				auto result = Packet{header};
				if constexpr(std::is_integral_v<decltype(sub)>)
					result.number = sub;
				else
					result.sub_packets = std::move(sub);
				return result;
			});
			while (is.peek() == '0')
				is.get();
			is >> Assert("\n");
		}
		else
			is.get();
		return is;
	}


	int read_sum_version(BitReader& bit_reader)
	{
		return read(bit_reader, [](const PacketHeader& header, auto sub) -> int
		{
			if constexpr(std::is_integral_v<decltype(sub)>)
				return header.version;
			else
				return accumulate(sub, static_cast<int>(header.version));
		});
	}


}


int q16a(std::istream& is)
{
	return accumulate(
		std::ranges::istream_view<Packet>(is) | std::views::transform(sum_versions), 0);
}


int q16b(std::istream& is)
{
	auto bit_reader = BitReader(is);
	return read_sum_version(bit_reader);
}
