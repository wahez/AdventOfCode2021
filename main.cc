#include "test_runner.h"
#include <chrono>
#include <istream>


int q01a(std::istream&);
int q01b(std::istream&);
int q02a(std::istream&);
int q02b(std::istream&);
int q03a(std::istream&);
int q03b(std::istream&);
int q04a(std::istream&);
int q04b(std::istream&);
int q05a(std::istream&);
int q05b(std::istream&);
int q06a(std::istream&);
std::int64_t q06b(std::istream&);
int q07a(std::istream&);
int q07b(std::istream&);
int q08a(std::istream&);
int q08b(std::istream&);
int q09a(std::istream&);


int main(int argc, char** argv)
{
	const auto start = std::chrono::steady_clock::now();
	const auto run = TestRunner("answers", argc, argv);
	run(q01a, "01a", "q01.tst", 7);
	run(q01a, "01a", "q01.inp");
	run(q01b, "01b", "q01.tst", 5);
	run(q01b, "01b", "q01.inp");
	run(q02a, "02a", "q02.tst", 150);
	run(q02a, "02a", "q02.inp");
	run(q02b, "02b", "q02.tst", 900);
	run(q02b, "02b", "q02.inp");
	run(q03a, "03a", "q03.tst", 198);
	run(q03a, "03a", "q03.inp");
	run(q03b, "03b", "q03.tst", 230);
	run(q03b, "03b", "q03.inp");
	run(q04a, "04a", "q04.tst", 4512);
	run(q04a, "04a", "q04.inp");
	run(q04b, "04b", "q04.tst", 1924);
	run(q04b, "04b", "q04.inp");
	run(q05a, "05a", "q05.tst", 5);
	run(q05a, "05a", "q05.inp");
	run(q05b, "05b", "q05.tst", 12);
	run(q05b, "05b", "q05.inp");
	run(q06a, "06a", "q06.tst", 5934);
	run(q06a, "06a", "q06.inp");
	run(q06b, "06b", "q06.tst", 26984457539);
	run(q06b, "06b", "q06.inp");
	run(q07a, "07a", "q07.tst", 37);
	run(q07a, "07a", "q07.inp");
	run(q07b, "07b", "q07.tst", 168);
	run(q07b, "07b", "q07.inp");
	run(q08a, "08a", "q08.tst", 26);
	run(q08a, "08a", "q08.inp");
	run(q08b, "08b", "q08.tst", 61229);
	run(q08b, "08b", "q08.inp");
	run(q09a, "09a", "q09.tst", 15);
	run(q09a, "09a", "q09.inp");
	const auto end = std::chrono::steady_clock::now();
	using namespace std::literals;
	std::cout << "Total : " << (end - start)/1ms << " ms" << std::endl;
}
