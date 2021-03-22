file(GLOB_RECURSE FAN_ENGINE_SRC *.cpp)
file(GLOB_RECURSE FAN_ENGINE_H *.hpp)

set( FAN_ENGINE_LIBS
    BulletDynamics
    BulletCollision
    LinearMath
    sfml-system
    sfml-network
    fanCore		
    fanRender
)

set( LIBRARY_NAME fanEngine )

add_library(${LIBRARY_NAME} STATIC ${FAN_ENGINE_SRC} ${FAN_ENGINE_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_ENGINE_LIBS})