file(GLOB_RECURSE FAN_NETWORK_SRC *.cpp)
file(GLOB_RECURSE FAN_NETWORK_H *.hpp)

set(FAN_NETWORK_LIBS
        sfml-system
        sfml-network
        fanEcs
        fanCore
        )

set(LIBRARY_NAME fanNetwork)

add_library(${LIBRARY_NAME} STATIC ${FAN_NETWORK_SRC} ${FAN_NETWORK_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_NETWORK_LIBS})