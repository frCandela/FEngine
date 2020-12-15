include_directories(
	imgui
	glslang
	bullet
	freetype
)

add_compile_options( ${MIDDLEWARE_COMPILATION_FLAGS} )

# imgui
file(GLOB_RECURSE IMGUI_SRC imgui/*.cpp)
file(GLOB_RECURSE IMGUI_H imgui/*.hpp)

add_library( imgui STATIC ${IMGUI_SRC} ${IMGUI_H})
target_link_libraries(imgui freetype28)

# quickhull
file(GLOB_RECURSE QUICKHULL_SRC quickhull/*.cpp)
file(GLOB_RECURSE QUICKHULL_H quickhull/*.hpp)
add_library( quickhull STATIC ${QUICKHULL_SRC} ${QUICKHULL_H})

# linear math
file(GLOB_RECURSE LINEAR_MATH_SRC bullet/LinearMath/*.cpp)
file(GLOB_RECURSE LINEAR_MATH_H bullet/LinearMath/*.hpp)
add_library( btLinearMath STATIC ${LINEAR_MATH_SRC} ${LINEAR_MATH_H})

# bullet dynamics
file(GLOB_RECURSE BT_DYNAMICS_SRC bullet/BulletDynamics/*.cpp)
file(GLOB_RECURSE BT_DYNAMICS_H bullet/BulletDynamics/*.hpp)
add_library( btDynamics STATIC ${BT_DYNAMICS_SRC} ${BT_DYNAMICS_H})

# bullet collision
file(GLOB_RECURSE BT_COLLISION_SRC bullet/BulletCollision/*.cpp)
file(GLOB_RECURSE BT_COLLISION_H bullet/BulletCollision/*.hpp)
add_library( btCollision STATIC ${BT_COLLISION_SRC} ${BT_COLLISION_H})
