set(HEADERS
	observable_ptr.hpp
)

add_library(observable_ptr

	${HEADERS}
)

target_include_directories(observable_ptr INTERFACE  ..)

set_target_properties(observable_ptr PROPERTIES LINKER_LANGUAGE CXX)
