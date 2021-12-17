#pragma once
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <string_view>


class TestRunner
{
public:
	explicit TestRunner(std::filesystem::path answers_path, int argc, char** argv)
	{
		auto answers_file = std::ifstream(answers_path);
		while (answers_file.good())
		{
			auto name = std::string{};
			auto answer = std::string{};
			answers_file >> name >> answer;
			if (answers_file.good())
				answers.emplace(name, answer);
		}
		for (int i = 1; i < argc; ++i)
			tests_to_run.insert(argv[i]);
	}

	void operator()(auto&& func, std::string_view name, const std::filesystem::path& path) const
	{
		const auto it = answers.find(name);
		if (it != answers.end())
		{
			using ReturnType = decltype(func(std::cin));
			if constexpr (std::is_integral_v<ReturnType>)
				(*this)(func, name, path, std::stoll(it->second));
			else// if constexpr (std::is_convertible_v<ReturnType, std::string>)
				(*this)(func, name, path, it->second);
		}
		else
		{
			if (!tests_to_run.empty() && !tests_to_run.contains(name))
			{
				std::cout << "Test " << name << " skipped" << std::endl;
				return;
			}
			const auto result = run(func, name, path);
			std::cout << "Test " << name << " produced " << result << std::endl;
		}
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
	std::map<std::string, std::string, std::less<>> answers;
};
