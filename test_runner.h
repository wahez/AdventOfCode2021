#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>
#include <string>
#include <string_view>


class TestRunner
{
public:
	explicit TestRunner(int argc, char** argv)
	{
		for (int i = 1; i < argc; ++i)
			tests_to_run.insert(argv[i]);
	}

	void operator()(auto&& func, std::string_view name, const std::filesystem::path& path) const
	{
		if (!tests_to_run.empty() && !tests_to_run.contains(name))
		{
			std::cout << "Test " << name << " skipped" << std::endl;
			return;
		}
		const auto result = run(func, name, path);
		std::cout << "Test " << name << " produced " << result << std::endl;
	}

	void operator()(auto&& func, std::string_view name, const std::filesystem::path& path, const auto& expected_output) const
	{
		if (!tests_to_run.empty() && !tests_to_run.contains(name))
		{
			std::cout << "Test " << name << " skipped" << std::endl;
			return;
		}
		const auto result = run(func, name, path);
		if (result != expected_output)
			std::cerr << "Test " << name << " failed with input (" << path << "), expected: " << expected_output << ", got: " << result << std::endl;
	}

private:
	static auto run(auto&& func, std::string_view name, const std::filesystem::path& path)
	{
		std::ifstream is("input" / path);
		if (!is.good())
			throw std::runtime_error("Could not find " + path.string());
		const auto start = std::chrono::steady_clock::now();
		const auto result = func(is);
		const auto end = std::chrono::steady_clock::now();
		using namespace std::literals;
		std::cout << "Test " << name << " ran in " << std::setw(6) << (end - start)/1us << " us with input " << path << std::endl;
		return result;
	}
	std::set<std::string, std::less<>> tests_to_run;
};
