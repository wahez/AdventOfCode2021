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


	struct Instruction
	{
		Direction direction;
		int x;
	};

	inline std::istream& operator>>(std::istream& is, Instruction& instruction)
	{
		is >> instruction.direction >> instruction.x;
		return is;
	}


	struct Position
	{
		int x;
		int depth;
		int aim = 0;
	};


}


int q02a(std::istream& is)
{
	auto pos = Position{0, 0};
	auto move = [&pos](const Instruction& instruction)
	{
		switch (instruction.direction)
		{
		case Direction::down:
			pos.depth += instruction.x;
			break;
		case Direction::forward:
			pos.x += instruction.x;
			break;
		case Direction::up:
			pos.depth -= instruction.x;
			break;
		}
	};
	auto instructions = std::ranges::istream_view<Instruction>(is);
	std::ranges::for_each(instructions, move);
	return pos.x * pos.depth;
}


int q02b(std::istream& is)
{
	auto pos = Position{0, 0, 0};
	auto move = [&pos](const Instruction& instruction)
	{
		switch (instruction.direction)
		{
		case Direction::down:
			pos.aim += instruction.x;
			break;
		case Direction::forward:
			pos.x += instruction.x;
			pos.depth += pos.aim * instruction.x;
			break;
		case Direction::up:
			pos.aim -= instruction.x;
			break;
		}
	};
	auto instructions = std::ranges::istream_view<Instruction>(is);
	std::ranges::for_each(instructions, move);
	return pos.x * pos.depth;
}
