file(GLOB_RECURSE FAN_CORE_SRC *.cpp)
file(GLOB_RECURSE FAN_CORE_H *.hpp)

set(FAN_CORE_LIBS
        imgui
        glfw
        quickhull
        )

set(LIBRARY_NAME fanCore)

add_library(${LIBRARY_NAME} STATIC ${FAN_CORE_SRC} ${FAN_CORE_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_CORE_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
