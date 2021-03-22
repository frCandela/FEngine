# glfw
set( GLFW_BUILD_DOCS OFF )
add_subdirectory(glfw)

# quickhull
add_library( quickhull STATIC quickhull/QuickHull.cpp )

# SFML
set( SFML_BUILD_WINDOW OFF )
set( SFML_BUILD_GRAPHICS OFF )
set( SFML_BUILD_AUDIO OFF )
add_compile_definitions(SFML_STATIC)
add_subdirectory(SFML)

# imgui
add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)
include_directories(imgui)
include_directories(sfml/extlibs/headers/freetype2)
file(GLOB_RECURSE IMGUI_MISC_SRC imgui/misc/*.cpp)
add_library( imgui STATIC ${IMGUI_MISC_SRC} imgui/imgui.cpp imgui/imgui_demo.cpp imgui/imgui_draw.cpp imgui/imgui_tables.cpp imgui/imgui_widgets.cpp)
target_link_libraries(imgui freetype)

# bullet3
set( CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set( BUILD_BULLET2_DEMOS OFF )
set( BUILD_CLSOCKET OFF )
set( BUILD_CPU_DEMOS OFF )
set( BUILD_ENET OFF )
set( BUILD_EXTRAS OFF )
set( BUILD_OPENGL3_DEMOS OFF )
set( BUILD_UNIT_TESTS OFF )
set( USE_MSVC_RUNTIME_LIBRARY_DLL ON )

add_subdirectory(bullet3)
