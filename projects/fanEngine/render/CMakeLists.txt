file(GLOB_RECURSE FAN_RENDER_SRC *.cpp)
file(GLOB_RECURSE FAN_RENDER_H *.hpp)

set(FAN_RENDER_LIBS
        vulkan-1
        fanCore
        )

set(LIBRARY_NAME fanRender)
add_library(${LIBRARY_NAME} STATIC ${FAN_RENDER_SRC} ${FAN_RENDER_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_RENDER_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
