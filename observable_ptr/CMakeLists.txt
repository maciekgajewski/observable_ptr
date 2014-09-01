set(HEADERS
	observable_ptr.hpp
)

add_library(observable_ptr

	${HEADERS}
)

set_target_properties(observable_ptr PROPERTIES LINKER_LANGUAGE CXX)
