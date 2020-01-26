cmake_minimum_required (VERSION 3.8)

project ("FEngine")
set(CMAKE_CXX_STANDARD 17)

set(APP_NAME fengine_editor)

include_directories(code/shaders)
include_directories(deps)
include_directories("C:/VulkanSDK/1.1.106.0/Include")

link_directories(lib/debug)

add_subdirectory(code)

list(APPEND APP_SOURCES
    code/main.cpp
) 

set(APP_LIBRARIES
    fanCore
    glfw3
)

add_executable(${APP_NAME} ${APP_SOURCES})

target_link_libraries(${APP_NAME} ${APP_LIBRARIES})

set_target_properties(fengine_editor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)


IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    MESSAGE(STATUS "Compilation mode  : Debug")
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Release)
    MESSAGE(STATUS "Compilation mode  : Release")
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Retail)
    #TODO
    MESSAGE(STATUS "Compilation mode  : Retail")
ENDIF()