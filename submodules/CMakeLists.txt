#add_compile_options( /W0 )

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

include_directories(bullet3/src)

# linear math
file(GLOB_RECURSE LINEAR_MATH_SRC bullet3/src/LinearMath/*.cpp)
file(GLOB_RECURSE LINEAR_MATH_H bullet3/src/LinearMath/*.hpp)
add_library( btLinearMath STATIC ${LINEAR_MATH_SRC} ${LINEAR_MATH_H})

# bullet dynamics
file(GLOB_RECURSE BT_DYNAMICS_SRC bullet3/src/BulletDynamics/*.cpp)
file(GLOB_RECURSE BT_DYNAMICS_H bullet3/src/BulletDynamics/*.hpp)
add_library( btDynamics STATIC ${BT_DYNAMICS_SRC} ${BT_DYNAMICS_H})

# bullet collision
file(GLOB_RECURSE BT_COLLISION_SRC bullet3/src/BulletCollision/*.cpp)
file(GLOB_RECURSE BT_COLLISION_H bullet3/src/BulletCollision/*.hpp)
add_library( btCollision STATIC ${BT_COLLISION_SRC} ${BT_COLLISION_H})
