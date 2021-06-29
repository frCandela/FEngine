file(GLOB_RECURSE FAN_EDITOR_SRC *.cpp)
file(GLOB_RECURSE FAN_EDITOR_H *.hpp)

set(FAN_EDITOR_LIBS
        fanCore
        fanEcs
        fanNetwork
        fanRender
        fanEngine
        )

set(LIBRARY_NAME fanEditor)
add_library(${LIBRARY_NAME} STATIC ${FAN_EDITOR_SRC} ${FAN_EDITOR_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_EDITOR_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
if (FAN_ENABLE_PCH)
    target_precompile_headers(${LIBRARY_NAME} PRIVATE ${PCH_DIR})
endif ()