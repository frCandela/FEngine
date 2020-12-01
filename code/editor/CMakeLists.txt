 include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_EDITOR_SRC *.cpp)
file(GLOB_RECURSE FAN_EDITOR_H *.hpp)

set( FAN_EDITOR_LIBS
    fanCore	
    fanRender
    fanNetwork
    fanEngine
)

add_compile_options( ${COMPILATION_FLAGS} )
add_library(fanEditor STATIC ${FAN_EDITOR_SRC} ${FAN_EDITOR_H})
target_link_libraries(fanEditor ${FAN_EDITOR_LIBS})