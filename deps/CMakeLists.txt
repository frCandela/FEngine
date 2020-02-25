include_directories(
	${CMAKE_SOURCE_DIR}/deps/imgui
	${CMAKE_SOURCE_DIR}/deps/glslang
)

# imgui
file(GLOB_RECURSE IMGUI_SRC imgui/*.cpp)
file(GLOB_RECURSE IMGUI_H imgui/*.hpp)
add_library( imgui STATIC ${IMGUI_SRC} ${IMGUI_H})
target_link_libraries(imgui freetype28)

# quickhull
file(GLOB_RECURSE QUICKHULL_SRC quickhull/*.cpp)
file(GLOB_RECURSE QUICKHULL_H quickhull/*.hpp)
add_library( quickhull STATIC ${QUICKHULL_SRC} ${QUICKHULL_H})

target_compile_options(quickhull PRIVATE /FUNCTIONPADMIN )
target_compile_options(imgui PRIVATE /FUNCTIONPADMIN )