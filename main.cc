#include "test_runner.h"
#include <chrono>
#include <istream>


int q01a(std::istream&);
int q01b(std::istream&);


int main(int argc, char** argv)
{
	const auto start = std::chrono::steady_clock::now();
	auto runner = TestRunner(argc, argv);
	runner.check(q01a, "01a", "q01.tst", 7);
	runner.check(q01a, "01a", "q01.inp", 1292);
	runner.check(q01b, "01b", "q01.tst", 5);
	runner.check(q01b, "01b", "q01.inp", 1262);
	const auto end = std::chrono::steady_clock::now();
	using namespace std::literals;
	std::cout << "Total : " << (end - start)/1ms << " ms" << std::endl;
}
