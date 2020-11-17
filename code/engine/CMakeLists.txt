 include_directories(
	${CMAKE_SOURCE_DIR}/code
    ${CMAKE_SOURCE_DIR}/middleware/bullet
)

file(GLOB_RECURSE FAN_ENGINE_SRC *.cpp)
file(GLOB_RECURSE FAN_ENGINE_H *.hpp)

set( FAN_ENGINE_LIBS
    fanCore		
    fanRender
    btDynamics
    btCollision
)

add_compile_options( ${COMPILATION_FLAGS} )
add_library(fanEngine STATIC ${FAN_ENGINE_SRC} ${FAN_ENGINE_H})
target_link_libraries(fanEngine ${FAN_ENGINE_LIBS})