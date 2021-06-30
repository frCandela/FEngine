file(GLOB_RECURSE FAN_PLATFORM_SRC *.cpp)
file(GLOB_RECURSE FAN_PLATFORM_H *.hpp)

set(FAN_PLATFORM_LIBS
        imgui
        glfw
        )

set(LIBRARY_NAME fanPlatform)

add_library(${LIBRARY_NAME} STATIC ${FAN_PLATFORM_SRC} ${FAN_PLATFORM_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_PLATFORM_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
if (FAN_ENABLE_PCH)
    target_precompile_headers(${LIBRARY_NAME} PRIVATE ${PCH_DIR})
endif ()