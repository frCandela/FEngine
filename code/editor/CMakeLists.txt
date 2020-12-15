file(GLOB_RECURSE FAN_EDITOR_SRC *.cpp)
file(GLOB_RECURSE FAN_EDITOR_H *.hpp)

set( FAN_EDITOR_LIBS
    fanCore	
    fanRender
    fanNetwork
    fanEngine
)

set( LIBRARY_NAME fanEditor )
add_library(${LIBRARY_NAME} STATIC ${FAN_EDITOR_SRC} ${FAN_EDITOR_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_EDITOR_LIBS})