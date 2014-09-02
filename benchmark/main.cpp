#include "benchmarks.hpp"

#include <iostream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <algorithm>



static constexpr std::size_t REPETITIONS = 1000;

template<typename Benchmark>
void run_becnhmark(std::size_t size)
{
	using clock = std::chrono::high_resolution_clock;
	using duration = clock::duration;

	std::vector<duration> nulls;
	std::vector<duration> measurements;

	nulls.reserve(REPETITIONS);
	measurements.reserve(REPETITIONS);

	for(std::size_t r = 0; r < REPETITIONS; r++)
	{
		Benchmark benchmark(size);
		auto start = clock::now();
		for(std::size_t i = 0; i < size; i++)
		{
			benchmark.null(i);
		}
		auto end = clock::now();
		nulls.push_back(start-end);
	}

	for(std::size_t r = 0; r < REPETITIONS; r++)
	{
		Benchmark benchmark(size);
		auto start = clock::now();
		for(std::size_t i = 0; i < size; i++)
		{
			benchmark.action(i);
		}
		auto end = clock::now();
		measurements.push_back(start-end);

		if (!benchmark.verify())
		{
			std::cerr << "Verification of benchmark " << typeid(Benchmark).name() << " " << size << " failed!" << std::endl;
			std::abort();
		}
	}

	std::sort(nulls.begin(), nulls.end());
	std::sort(measurements.begin(), measurements.end());

	duration median_corrected = measurements[REPETITIONS/2] - nulls[0];
	duration min_corrected = measurements[0] - nulls[0];


	std::cout << size << '\t' << min_corrected/std::chrono::nanoseconds(1) << '\t' << median_corrected/std::chrono::nanoseconds(1) << std::endl;
}

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		std::cerr << "Usage: benchmark TYPE SIZE" << std::endl;
		std::cerr << "sizeof shared_ptr: " << sizeof(std::shared_ptr<int>) << std::endl;
		std::cerr << "sizeof weak_ptr: " << sizeof(std::weak_ptr<int>) << std::endl;
		std::cerr << "sizeof observable_ptr: " << sizeof(observable_ptr<int>) << std::endl;
		std::cerr << "sizeof observer_ptr: " << sizeof(observer_ptr<int>) << std::endl;
		return 1;
	}

	std::string type = argv[1];
	std::size_t size = std::stoull(argv[2]);

	if (type == "create_observable")
	{
		run_becnhmark<create_observable>(size);
	}
	else if (type == "create_shared")
	{
		run_becnhmark<create_shared>(size);
	}
	else if (type == "read_shared")
	{
		run_becnhmark<read_shared>(size);
	}
	else if (type == "read_observable")
	{
		run_becnhmark<read_observable>(size);
	}
}


