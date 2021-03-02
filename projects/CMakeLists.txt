if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

add_compile_definitions(GLFW_INCLUDE_VULKAN)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/fengine
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

add_subdirectory(fengine)
add_subdirectory(project_spaceships/code)
add_subdirectory(project_empty/code)