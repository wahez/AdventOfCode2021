#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <variant>
#include <vector>
#include "util.h"


namespace
{


	template<class... Ts> struct overload : Ts... { using Ts::operator()...; };


	struct ExplodeResult
	{
		bool exploded = false;
		std::optional<int> left;
		std::optional<int> right;

		explicit operator bool() const { return exploded; }
	};

	struct SnailNumber
	{
		using NodeType = std::variant<int, std::unique_ptr<SnailNumber>>;
		NodeType left;
		NodeType right;

		SnailNumber copy() const
		{
			auto copy_node = overload{
				[](int n) -> NodeType { return n; },
				[](const auto& n) -> NodeType { return std::make_unique<SnailNumber>(n->copy()); },
			};
			auto sn = SnailNumber{};
			sn.left = std::visit(copy_node, left);
			sn.right = std::visit(copy_node, right);
			return sn;
		}

		void reduce()
		{
			while (explode(1) || split());
		}

		int magnitude() const
		{
			auto part = overload{
				[](int n) { return n; },
				[](const auto& n) { return n->magnitude(); },
			};
			return 3*std::visit(part, left) + 2*std::visit(part, right);
		}

		ExplodeResult explode(int depth)
		{
			auto explode_child = overload{
				[depth](int n) -> ExplodeResult { return {false, {}, {}}; },
				[depth](auto& n) -> ExplodeResult { return n->explode(depth+1); },
			};
			if (auto result = std::visit(explode_child, left); result)
			{
				if (result.right)
				{
					if (result.left)
						left = 0;
					add_left(*result.right, right);
				}
				result.right = {};
				return result;
			}
			if (auto result = std::visit(explode_child, right); result)
			{
				if (result.left)
				{
					if (result.right)
						right = 0;
					add_right(*result.left, left);
				}
				result.left = {};
				return result;
			}
			if (depth > 4)
				return {true, std::get<int>(left), std::get<int>(right)};
			return {false};
		}

		bool split()
		{
			auto split_child = [](auto& node)
			{
				return std::visit(overload{
						[&](int n)
						{
							if (n < 10)
								return false;
							else
							{
								node = std::make_unique<SnailNumber>(n/2, (n+1)/2);
								return true;
							}
						},
						[&](auto& n) { return n->split(); },
					},
					node);
			};
			return split_child(left) || split_child(right);
		}
	private:
		static void add_left(int number, std::variant<int, std::unique_ptr<SnailNumber>>& snail_number)
		{
			std::visit(overload{
					[number](int& node) { node += number; },
					[number](auto& node) { add_left(number, node->left); },
				},
				snail_number);
		};

		static void add_right(int number, std::variant<int, std::unique_ptr<SnailNumber>>& snail_number)
		{
			std::visit(overload{
					[number](int& node) { node += number; },
					[number](auto& node) { add_right(number, node->right); },
				},
				snail_number);
		};
	};

	std::ostream& operator<<(std::ostream& os, SnailNumber& snail_number)
	{
		auto print_sub = overload{
			[&](int n) { os << n; },
			[&](const auto& n) { os << *n; },
		};
		os << '[';
		std::visit(print_sub, snail_number.left);
		os << ',';
		std::visit(print_sub, snail_number.right);
		os << ']';
		return os;
	}

	std::istream& operator>>(std::istream& is, SnailNumber& snail_number)
	{
		auto read_part = [&]() -> std::variant<int, std::unique_ptr<SnailNumber>>
		{
			if (is.peek() == '[')
				return std::make_unique<SnailNumber>(read<SnailNumber>(is));
			else
				return read<int>(is);
		};
		is >> Assert('[');
		snail_number.left = read_part();
		is >> Assert(',');
		snail_number.right = read_part();
		is >> Assert(']');
		return is;
	}


	SnailNumber operator+(SnailNumber lhs, SnailNumber rhs)
	{
		auto snail_number = SnailNumber{std::make_unique<SnailNumber>(std::move(lhs)), std::make_unique<SnailNumber>(std::move(rhs))};
		snail_number.reduce();
		return snail_number;
	}


}


int q18a(std::istream& is)
{
	auto numbers = std::ranges::istream_view<SnailNumber>(is);
	auto it = numbers.begin();
	auto total = std::move(*it);
	is >> Assert('\n');
	for (++it; it != numbers.end(); ++it)
	{
		total = std::move(total) + std::move(*it);
		is >> Assert('\n');
		if (is.peek() == '\n')
			break;
	}
	return total.magnitude();
}


int q18b(std::istream& is)
{
	auto numbers = std::vector<SnailNumber>{};
	for (auto& sn: std::ranges::istream_view<SnailNumber>(is))
	{
		numbers.push_back(std::move(sn));
		is >> Assert('\n');
		if (is.peek() == '\n')
			break;
	}
	auto max = 0;
	for (auto i1 = 0; i1 < std::ssize(numbers); ++i1)
		for (auto i2 = 0; i2 < std::ssize(numbers); ++i2)
			if (i1 != i2)
				max = std::max(max, (numbers[i1].copy()+numbers[i2].copy()).magnitude());
	return max;
}
