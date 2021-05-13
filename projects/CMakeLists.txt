set(CMAKE_CXX_STANDARD 20)

# options
set(VULKAN_DIR C:/VulkanSDK/1.2.162.0)
set(LIVEPP_DIR C:/LivePP)
set(LIVEPP_ENABLE ON)

link_directories("${VULKAN_DIR}/Lib")
if (${CMAKE_BUILD_TYPE} MATCHES Debug)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/debug)
    if (MSVC)
        add_compile_options(/FUNCTIONPADMIN)
    endif ()
    add_compile_definitions(FAN_DEBUG)
elseif (${CMAKE_BUILD_TYPE} MATCHES Release)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/release)
    set(LIVEPP_ENABLE OFF)
    add_compile_definitions(FAN_RELEASE)
endif ()
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#compiler
if (MSVC)
    link_directories(${CMAKE_SOURCE_DIR}/submodules/SFML/extlibs/libs-msvc/x64)
    add_compile_options(/W4 /WX)
    add_compile_definitions(FAN_MSVC)
elseif (MINGW)
    link_directories(${CMAKE_SOURCE_DIR}/submodules/SFML/extlibs/libs-mingw/x64)
    add_compile_options(-Wall -Werror -Wextra -pedantic)
    add_compile_definitions(FAN_MINGW)
endif ()

#platform
add_compile_definitions(FAN_VULKAN_PATH="${VULKAN_DIR}")
if (WIN32)
    add_compile_definitions(FAN_WIN32)
endif ()

#live++
if (LIVEPP_ENABLE)
    add_compile_definitions(FAN_LIVEPP_PATH=L"${LIVEPP_DIR}/LivePP")
    add_compile_definitions(FAN_LIVEPP)
    include_directories(${LIVEPP_DIR})
endif ()

add_compile_definitions(GLFW_INCLUDE_VULKAN)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/fanEngine
        ${CMAKE_SOURCE_DIR}/submodules/SFML/extlibs/headers/freetype2
        ${CMAKE_SOURCE_DIR}/submodules/SFML/include
        ${CMAKE_SOURCE_DIR}/submodules/glfw/include
        ${CMAKE_SOURCE_DIR}/submodules/glm
        ${CMAKE_SOURCE_DIR}/submodules/bullet3/src
        ${CMAKE_SOURCE_DIR}/submodules/json/include
        ${CMAKE_SOURCE_DIR}/submodules/stb
        ${CMAKE_SOURCE_DIR}/submodules
        ${VULKAN_DIR}/Include
)

add_subdirectory(fanEngine)
add_subdirectory(empty_game)
add_subdirectory(darkreign3)