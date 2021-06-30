file(GLOB_RECURSE FAN_ENGINE_SRC *.cpp)
file(GLOB_RECURSE FAN_ENGINE_H *.hpp)

set(FAN_ENGINE_LIBS
        sfml-network
        sfml-system
        fanPlatform
        fanCore
        fanEcs
        fanRender
        )

set(LIBRARY_NAME fanEngine)

add_library(${LIBRARY_NAME} STATIC ${FAN_ENGINE_SRC} ${FAN_ENGINE_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_ENGINE_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
if (FAN_ENABLE_PCH)
    target_precompile_headers(${LIBRARY_NAME} PRIVATE ${PCH_DIR})
endif ()