cmake_minimum_required (VERSION 3.8)
project ("FEngine")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set( VULKAN_DIR C:/VulkanSDK/1.2.162.0)
set( SHADERC_DIR C:/VulkanSDK/shaderc)

link_directories("${VULKAN_DIR}/Lib")
link_directories(submodules/SFML/extlibs/libs-msvc/x64)
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    link_directories(${SHADERC_DIR}/debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug )
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug )
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Release)
    link_directories(${SHADERC_DIR}/release)
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

#projects directories
add_subdirectory(submodules)
add_subdirectory(middleware)
add_subdirectory(code)
