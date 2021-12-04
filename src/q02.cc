#include <algorithm>
#include <istream>
#include <ranges>


namespace {


	enum class Direction
	{
		down,
		forward,
		up,
	};

	struct Instruction
	{
		Direction direction;
		int distance;
	};


	inline std::istream& operator>>(std::istream& is, Direction& direction)
	{
		std::string word;
		is >> word;
		if (word == "down")
			direction = Direction::down;
		else if (word == "forward")
			direction = Direction::forward;
		else if (word == "up")
			direction = Direction::up;
		else if (is.good())
			throw std::runtime_error("Unexpected direction: " + word);
		return is;
	}

	inline std::istream& operator>>(std::istream& is, Instruction& instruction)
	{
		is >> instruction.direction >> instruction.distance;
		return is;
	}


	struct Position
	{
		int x;
		int depth;
	};

	Position operator+(Position pos, Instruction instruction)
	{
		switch (instruction.direction)
		{
		case Direction::down:
			pos.depth += instruction.distance;
			break;
		case Direction::forward:
			pos.x += instruction.distance;
			break;
		case Direction::up:
			pos.depth -= instruction.distance;
			break;
		}
		return pos;
	}


	template<typename Op = std::plus<>>
	auto accumulate(std::ranges::input_range auto&& range, auto init, Op&& op = Op{})
	{
		std::ranges::for_each(
				range,
				[&](const auto& element)
				{
					init = std::move(init) + element;
				});
		return init;
	}


}


int q02a(std::istream& is)
{
	const auto pos = accumulate(
		std::ranges::istream_view<Instruction>(is),
		Position{0, 0});
	return pos.x * pos.depth;
}
