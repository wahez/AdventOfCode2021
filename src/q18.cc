#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <variant>
#include "util.h"


namespace
{


	struct ExplodeResult
	{
		bool exploded = false;
		std::optional<int> left;
		std::optional<int> right;

		explicit operator bool() const { return exploded; }
	};

	struct SnailNumber
	{
		std::variant<int, std::unique_ptr<SnailNumber>> left;
		std::variant<int, std::unique_ptr<SnailNumber>> right;

		void reduce()
		{
			while (explode(1) || split());
		}

		int magnitude() const
		{
			auto part = [](const auto& n)
			{
				if constexpr(std::is_same_v<int, std::decay_t<decltype(n)>>)
					return n;
				else
					return n->magnitude();
			};
			return 3*std::visit(part, left) + 2*std::visit(part, right);
		}

		ExplodeResult explode(int depth)
		{
			auto explode_child = [](auto& node, int depth)
				{
					return std::visit([depth](auto& n) -> ExplodeResult
						{
							if constexpr(std::is_same_v<int, std::decay_t<decltype(n)>>)
								return {false, {}, {}};
							else
								return n->explode(depth+1);
						},
						node);
				};
			if (auto result = explode_child(left, depth); result)
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
			if (auto result = explode_child(right, depth); result)
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
					return std::visit([&](auto& n)
						{
							if constexpr(std::is_same_v<int, std::decay_t<decltype(n)>>)
							{
								if (n < 10)
									return false;
								else
								{
									node = std::make_unique<SnailNumber>(n/2, (n+1)/2);
									return true;
								}
							}
							else
								return n->split();
						},
						node);
				};
			return split_child(left) || split_child(right);
		}
	private:
		static void add_left(int number, std::variant<int, std::unique_ptr<SnailNumber>>& snail_number)
		{
			std::visit([number](auto& node)
				{
					if constexpr(std::is_same_v<int, std::decay_t<decltype(node)>>)
						node += number;
					else
						add_left(number, node->left);
				},
				snail_number);
		};

		static void add_right(int number, std::variant<int, std::unique_ptr<SnailNumber>>& snail_number)
		{
			std::visit([number](auto& node)
				{
					if constexpr(std::is_same_v<int, std::decay_t<decltype(node)>>)
						node += number;
					else
						add_right(number, node->right);
				},
				snail_number);
		};
	};

	std::ostream& operator<<(std::ostream& os, SnailNumber& snail_number)
	{
		auto print_sub = [&](const auto& n)
		{
			if constexpr(std::is_same_v<int, std::decay_t<decltype(n)>>)
				os << n;
			else
				os << *n;
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
