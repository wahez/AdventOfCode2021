#pragma once
#include <algorithm>
#include <charconv>
#include <ranges>
#include <sstream>
#include <string_view>


template<typename Op = std::plus<>>
auto accumulate(std::ranges::input_range auto&& range, auto init, Op&& op = Op{})
{
	std::ranges::for_each(
				range,
				[&](const auto& element)
	{
		init = op(std::move(init), element);
	});
	return init;
}


template<typename T>
T read(std::istream& is) { auto result = T{}; is >> result; return result; }


template<typename T>
struct Assert
{
	T expected;
};

template<typename T>
std::istream& operator>>(std::istream& is, const Assert<T>& a)
{
	const auto found = read<T>(is);
	if (found != a.expected && is.good())
	{
		std::ostringstream os;
		os << "Expected \"" << a.expected << "\", found \"" << found << "\"";
		throw std::runtime_error(os.str());
	}
	return is;
}

template<>
inline std::istream& operator>>(std::istream& is, const Assert<char>& a)
{
	const auto found = is.get();
	if (found != a.expected && is.good())
	{
		std::ostringstream os;
		os << "Expected \"" << a.expected << "\", found \"" << found << "\"";
		throw std::runtime_error(os.str());
	}
	return is;
}

template<>
inline std::istream& operator>>(std::istream& is, const Assert<const char*>& a)
{
	const auto a_view = std::string_view{a.expected};
	for (const char expected: a_view)
	{
		const auto found = is.get();
		if (found != expected && is.good())
		{
			std::ostringstream os;
			os << "Expected \"" << a.expected << "\", found \"" << found << "\"";
			throw std::runtime_error(os.str());
		}
	}
	return is;
}


template<typename T>
struct NotChar
{
	T& target;
};

template<typename T>
inline std::istream& operator>>(std::istream& is, const NotChar<T>& a)
{
	const auto value = read<std::int64_t>(is);
	a.target = static_cast<T>(value);
	return is;
}

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const NotChar<T>& a)
{
	os << static_cast<std::int64_t>(a.target);
	return os;
}


template<typename Container>
auto read_separated(std::istream& is, const char separator = ',', const char end = '\n')
{
	Container result;
	while (true)
	{
		auto value = read<typename Container::value_type>(is);
		if (!is.good())
			break;
		result.push_back(std::move(value));
		const auto next = is.get();
		if (next == end)
			break;
		else if (next != separator)
			throw std::runtime_error("Expected separator");
	}
	return result;
}


template<typename Container>
Container read_non_separated(std::istream& is)
{
	auto result = Container{};
	auto view = std::ranges::istream_view<typename Container::value_type>(is);
	std::ranges::move(view, std::back_inserter(result));
	return result;
}
