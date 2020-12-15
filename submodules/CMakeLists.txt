# glfw
set( GLFW_BUILD_DOCS OFF )
add_subdirectory(glfw)

# quickhull
add_library( quickhull STATIC quickhull/QuickHull.cpp )

#SFML
set( SFML_BUILD_WINDOW OFF )
set( SFML_BUILD_GRAPHICS OFF )
set( SFML_BUILD_AUDIO OFF )
add_compile_definitions(SFML_STATIC)
add_subdirectory(SFML)