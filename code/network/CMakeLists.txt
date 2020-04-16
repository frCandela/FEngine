include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_NETWORK_SRC *.cpp)
file(GLOB_RECURSE FAN_NETWORK_H *.hpp)

set(FAN_NETWORK_LIBS
    fanCore
	fanEcs
	ws2_32
	sfml-system-s
	sfml-network-s
)

add_compile_definitions(SFML_STATIC)

add_library(fanNetwork STATIC ${FAN_NETWORK_SRC} ${FAN_NETWORK_H})

target_link_libraries(fanNetwork ${FAN_NETWORK_LIBS})