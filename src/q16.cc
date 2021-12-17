#include <algorithm>
#include <istream>
#include <ranges>
#include <vector>
#include <cassert>
#include "util.h"


namespace
{


	auto shift_in(auto value, auto new_bits, auto num_bits)
	{
		const auto mask = (1<<num_bits)-1;
		return (value << num_bits) | (new_bits & mask);
	}


	class BitReader
	{
	public:
		explicit BitReader(std::istream& i) : is(i) {}

		auto get_bits_consumed() const { return bits_consumed; }

		template<typename T=int>
		T consume(int num_bits)
		{
			assert(sizeof(T)*8 >= static_cast<std::size_t>(num_bits));
			bits_consumed += num_bits;
			for (; cached_bits_in_use < num_bits; cached_bits_in_use += 4)
			{
				const auto c = is.get();
				if (!is.good() || c == EOF)
					throw std::runtime_error("EOF");
				else if (c > '9')
					cached_bits = shift_in(cached_bits, c - 'A' + 10, 4);
				else
					cached_bits = shift_in(cached_bits, c - '0', 4);
			}
			const auto mask = (1<<num_bits)-1;
			cached_bits_in_use -= num_bits;
			return (cached_bits >> cached_bits_in_use) & mask;
		}

		template<typename T>
		void consume(int num_bits, T& target) { target = static_cast<T>(consume<int>(num_bits)); }

	private:
		std::istream& is;
		std::uint64_t cached_bits = 0;
		std::int8_t cached_bits_in_use = 0;
		int bits_consumed = 0;
	};


	enum class TypeID : std::int8_t
	{
		SUM = 0,
		PRODUCT = 1,
		MINIMUM = 2,
		MAXIMUM = 3,
		LITERAL = 4,
		GREATER = 5,
		LESS = 6,
		EQUAL = 7,
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
	};


	auto read_literal(BitReader& bit_reader)
	{
		auto value = 0ll;
		auto continue_bit = 0;
		do
		{
			bit_reader.consume(1, continue_bit);
			auto next_bits = bit_reader.consume(4);
			value = shift_in(value, next_bits, 4);
		}
		while (continue_bit);
		return value;
	}

	auto parse_packets(BitReader& bit_reader, auto&& process_packet)
	{
		using ResultType = decltype(process_packet(PacketHeader{}, 0));
		auto header = PacketHeader{};
		bit_reader.consume(3, header.version);
		bit_reader.consume(3, header.type_id);
		if (header.type_id == TypeID::LITERAL)
			return process_packet(header, read_literal(bit_reader));
		else
		{
			auto sub_results = std::vector<ResultType>{};
			auto length_type_id = LengthTypeID{};
			bit_reader.consume(1, length_type_id);
			switch (length_type_id)
			{
			case LengthTypeID::NUM_BITS:
			{
				const auto num_bits = bit_reader.consume(15);
				const auto current_bits_consumed = bit_reader.get_bits_consumed();
				while (bit_reader.get_bits_consumed() < current_bits_consumed + num_bits)
					sub_results.push_back(parse_packets(bit_reader, process_packet));
				break;
			}
			case LengthTypeID::NUM_PACKETS:
			{
				const auto num_sub_packets = bit_reader.consume(11);
				for (auto i = 0; i < num_sub_packets; ++i)
					sub_results.push_back(parse_packets(bit_reader, process_packet));
				break;
			}
			}
			return process_packet(header, std::move(sub_results));
		}
	}


}


int q16a(std::istream& is)
{
	auto bit_reader = BitReader(is);
	return parse_packets(bit_reader, [&]<typename PacketData>(const PacketHeader& header, PacketData data) -> int
	{
		if constexpr(std::is_integral_v<PacketData>)
			return header.version;
		else
			return accumulate(data, static_cast<int>(header.version));
	});
}


std::int64_t q16b(std::istream& is)
{
	auto bit_reader = BitReader(is);
	return parse_packets(bit_reader, [&]<typename PacketData>(const PacketHeader& header, PacketData data) -> std::int64_t
	{
		if constexpr(std::is_integral_v<PacketData>)
		{
			assert(header.type_id == TypeID::LITERAL);
			return data;
		}
		else
		{
			switch (header.type_id)
			{
			case TypeID::SUM    : return accumulate(data, 0ll, std::plus<>{});
			case TypeID::PRODUCT: return accumulate(data, 1ll, std::multiplies<>{});
			case TypeID::MINIMUM: return *std::ranges::min_element(data);
			case TypeID::MAXIMUM: return *std::ranges::max_element(data);
			case TypeID::LITERAL: throw std::runtime_error("LITERAL with sub packets");
			case TypeID::GREATER: assert(data.size() == 2); return data[0] > data[1];
			case TypeID::LESS   : assert(data.size() == 2); return data[0] < data[1];
			case TypeID::EQUAL  : assert(data.size() == 2); return data[0] == data[1];
			default: throw std::runtime_error("Unknown TypeID");
			}
		}
	});
}
