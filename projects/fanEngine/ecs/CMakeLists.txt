file(GLOB_RECURSE FAN_ECS_SRC *.cpp)
file(GLOB_RECURSE FAN_ECS_H *.hpp)

set(FAN_ECS_LIBS
        fanCore
        )

set(LIBRARY_NAME fanEcs)

add_library(${LIBRARY_NAME} STATIC ${FAN_ECS_SRC} ${FAN_ECS_H})
target_link_libraries(${LIBRARY_NAME} ${FAN_ECS_LIBS})
set_target_properties(${LIBRARY_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})
