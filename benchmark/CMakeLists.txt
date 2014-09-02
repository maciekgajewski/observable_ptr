
add_executable(benchmark

	main.cpp

	benchmarks.hpp
)

target_link_libraries(benchmark
	observable_ptr
)

