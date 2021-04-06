cmake_minimum_required (VERSION 3.0)

set (CMAKE_CXX_STANDARD 17)

project (raytrace)

include_directories(
	external/glm-0.9.7.1/
	.
)

file(GLOB src
	"src/main.cpp"
)

add_executable(raytrace
	${src}
)
target_link_libraries(raytrace
	${ALL_LIBS}
)


