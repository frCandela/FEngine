 include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_ECS_SRC *.cpp)
file(GLOB_RECURSE FAN_ECS_H *.hpp)

set( FAN_ECS_LIBS
    fanCore
)

add_compile_options( ${COMPILATION_FLAGS} )
add_library(fanEcs STATIC ${FAN_ECS_SRC} ${FAN_ECS_H})
target_link_libraries(fanEcs ${FAN_ECS_LIBS})