cmake_minimum_required(VERSION 3.8)

project("FEngine")

#Removes default MSVC warning setting
if (MSVC)
    if (CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    endif ()
endif ()

add_subdirectory(submodules)
add_subdirectory(projects)