file(GLOB_RECURSE FAN_NETWORK_SRC *.cpp)
file(GLOB_RECURSE FAN_NETWORK_H *.hpp)

set(FAN_NETWORK_LIBS
        sfml-system
        sfml-network
        fanPlatform
        fanCore
        fanEcs
        )

set(LIBRARY_NAME fanNetwork)

add_library(${LIBRARY_NAME} STATIC ${FAN_NETWORK_SRC} ${FAN_NETWORK_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_NETWORK_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
if (FAN_ENABLE_PCH)
    target_precompile_headers(${LIBRARY_NAME} PRIVATE ${PCH_DIR})
endif ()
