#include <algorithm>
#include <bitset>
#include <istream>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>
#include <cassert>
#include "util.h"


namespace {


	struct Bitset
	{
		std::bitset<31> bits;
		int num_bits;
	};

	std::istream& operator>>(std::istream& is, Bitset& bitset)
	{
		std::string s;
		is >> s;
		if (s.size() > bitset.bits.size())
			throw std::runtime_error("Too many bits");
		bitset.num_bits = s.size();
		bitset.bits = std::bitset<31>(s);
		return is;
	}


	auto find_most_common_bits(const auto& inputs)
	{
		auto bits = 0;
		for (int i = inputs[0].num_bits - 1; i >= 0; --i)
		{
			const auto num_ones = std::ranges::count_if(
				inputs,
				[&](const auto& input)
				{
					return input.bits[i] == 1;
				});
			bits = (bits << 1) | (2*num_ones < std::ssize(inputs));
		}
		return bits;
	}


	auto find_best_match(auto inputs, bool keep_one)
	{
		auto inputs_tmp	= decltype(inputs){};
		for (int i = inputs[0].num_bits - 1; i >= 0; --i)
		{
			const auto count = std::ranges::count_if(
				inputs,
				[&](const auto& input)
				{
					return input.bits[i] == 1;
				});
			const bool expected_value = keep_one ^ (2*count < std::ssize(inputs));
			std::ranges::copy_if(
				inputs,
				std::back_inserter(inputs_tmp),
				[&](const auto& input)
				{
					return input.bits[i] == expected_value;
				});
			std::swap(inputs, inputs_tmp);
			inputs_tmp.clear();
			if (inputs.size() == 1)
				return inputs[0];
		}
		throw std::runtime_error("Could not find best match");
	}


}


int q03a(std::istream& is)
{
	auto inputs = read_non_separated<std::vector<Bitset>>(is);
	const auto gamma = find_most_common_bits(inputs);
	const auto epsilon = (1 << inputs[0].num_bits) - 1 - gamma;
	return gamma * epsilon;
}


int q03b(std::istream& is)
{
	auto inputs = read_non_separated<std::vector<Bitset>>(is);
	const auto oxygen_generator_rating = find_best_match(inputs, true).bits.to_ulong();
	const auto co2_scrubber_rating = find_best_match(inputs, false).bits.to_ulong();
	return oxygen_generator_rating * co2_scrubber_rating;
}
