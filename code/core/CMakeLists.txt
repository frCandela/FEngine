include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_CORE_SRC *.cpp)
file(GLOB_RECURSE FAN_CORE_H *.hpp)

set(FAN_CORE_LIBS
	imgui
    glfw3
	LinearMath_Debug 
	quickhull
)

add_library(fanCore STATIC ${FAN_CORE_SRC} ${FAN_CORE_H})

target_link_libraries(fanCore ${FAN_CORE_LIBS})
