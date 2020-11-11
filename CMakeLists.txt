cmake_minimum_required (VERSION 3.8)

project ("FEngine")
set(CMAKE_CXX_STANDARD 17)

# FORCE C++17 STANDARD
set_target_properties(${APP_NAME} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

set( APP_NAME "fengine_editor")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

include_directories( code )
include_directories( middleware )
include_directories( middleware/freetype )
include_directories( "C:/VulkanSDK/1.1.106.0/Include" )

link_directories("C:/VulkanSDK/1.1.106.0/Lib")
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    link_directories(lib/debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug )
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Release)
    link_directories(lib/release)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release )
ENDIF()

if (MSVC)
    #Removes default warning setting
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()
    set( COMPILATION_FLAGS /W4 /WX  )
    set( MIDDLEWARE_COMPILATION_FLAGS /W0 )
    add_compile_options( /FUNCTIONPADMIN )
endif ()

set( CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin )
add_compile_definitions(SFML_STATIC)
add_compile_definitions(GLFW_INCLUDE_VULKAN)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)
add_compile_definitions(IMGUI_DISABLE_OBSOLETE_FUNCTIONS)

# libs directories
add_subdirectory(middleware)
add_subdirectory(code/core)
add_subdirectory(code/render)
add_subdirectory(code/network)
add_subdirectory(code/scene)
add_subdirectory(code/game)
add_subdirectory(code/editor)

# sources
file( GLOB_RECURSE LPP_SRC deps/LivePP/*.h )
list(APPEND APP_SOURCES
    code/fanMain.cpp
    ${LPP_SRC}    
) 

set(APP_LIBRARIES 
    fanCore
    fanRender
    fanNetwork
    fanScene
    fanGame
    fanEditor
)

if ( ${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC  )
    add_executable(${APP_NAME} WIN32 ${APP_SOURCES} )
else()
    add_executable(${APP_NAME} ${APP_SOURCES} )
endif ()


target_link_libraries(${APP_NAME} ${APP_LIBRARIES})
target_compile_options(${APP_NAME} PRIVATE ${COMPILATION_FLAGS} )