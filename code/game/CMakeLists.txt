 include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_GAME_SRC *.cpp)
file(GLOB_RECURSE FAN_GAME_H *.hpp)

set( FAN_SCENE_LIBS
    fanCore		
    fanRender
    LinearMath_Debug 
    BulletDynamics_Debug
    BulletCollision_Debug
    fanScene
)

add_library(fanGame STATIC ${FAN_GAME_SRC} ${FAN_GAME_H})

target_link_libraries(fanGame ${FAN_SCENE_LIBS})
