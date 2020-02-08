 include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_SCENE_SRC *.cpp)
file(GLOB_RECURSE FAN_SCENE_H *.hpp)

set( FAN_SCENE_LIBS
    fanCore		
    fanRender
    BulletDynamics
    BulletCollision
)

add_library(fanScene STATIC ${FAN_SCENE_SRC} ${FAN_SCENE_H})

target_link_libraries(fanScene ${FAN_SCENE_LIBS})