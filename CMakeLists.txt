cmake_minimum_required (VERSION 3.8)

project ("FEngine")
set(CMAKE_CXX_STANDARD 17)

set(APP_NAME fengine_editor)

include_directories(deps)
include_directories(code)
include_directories("C:/VulkanSDK/1.1.106.0/Include")

link_directories(lib/debug)
link_directories("C:/VulkanSDK/1.1.106.0/Lib")

add_subdirectory(code/core)
add_subdirectory(code/render)
add_subdirectory(code/network)
add_subdirectory(code/scene)
add_subdirectory(code/game)
add_subdirectory(code/editor)
add_subdirectory(deps)

add_compile_definitions(SFML_STATIC)

list(APPEND APP_SOURCES
    code/editor/fanMain.cpp
) 

set(APP_LIBRARIES 
    fanCore
    fanRender
    fanNetwork
    fanScene
    fanGame
    fanEditor
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(${APP_NAME} ${APP_SOURCES})

target_link_libraries(${APP_NAME} ${APP_LIBRARIES})

# FORCE C++17 STANDARD
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