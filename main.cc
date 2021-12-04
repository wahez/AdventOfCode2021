#include "test_runner.h"
#include <chrono>
#include <istream>


int q01a(std::istream&);
int q01b(std::istream&);
int q02a(std::istream&);
int q02b(std::istream&);
int q03a(std::istream&);
int q03b(std::istream&);


int main(int argc, char** argv)
{
	const auto start = std::chrono::steady_clock::now();
	const auto run = TestRunner(argc, argv);
	run(q01a, "01a", "q01.tst", 7);
	run(q01a, "01a", "q01.inp", 1292);
	run(q01b, "01b", "q01.tst", 5);
	run(q01b, "01b", "q01.inp", 1262);
	run(q02a, "02a", "q02.tst", 150);
	run(q02a, "02a", "q02.inp", 2039912);
	run(q02b, "02b", "q02.tst", 900);
	run(q02b, "02b", "q02.inp", 1942068080);
	run(q03a, "03a", "q03.tst", 198);
	run(q03a, "03a", "q03.inp", 2250414);
	run(q03b, "03b", "q03.tst", 230);
	run(q03b, "03b", "q03.inp", 6085575);
	const auto end = std::chrono::steady_clock::now();
	using namespace std::literals;
	std::cout << "Total : " << (end - start)/1ms << " ms" << std::endl;
}
