 include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_GAME_SRC *.cpp)
file(GLOB_RECURSE FAN_GAME_H *.hpp)

set( FAN_GAME_LIBS
    fanCore
    fanRender
    fanNetwork
    fanScene
)

add_compile_definitions(SFML_STATIC)

add_library(fanGame STATIC ${FAN_GAME_SRC} ${FAN_GAME_H})

target_link_libraries(fanGame ${FAN_GAME_LIBS})