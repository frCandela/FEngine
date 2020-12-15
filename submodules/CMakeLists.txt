# glfw
set( GLFW_BUILD_DOCS OFF )
add_subdirectory(glfw)

# quickhull
add_library( quickhull STATIC quickhull/QuickHull.cpp )