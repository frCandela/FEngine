add_compile_definitions(GLFW_INCLUDE_VULKAN)
add_compile_definitions(GLM_FORCE_RADIANS)
add_compile_definitions(GLM_FORCE_DEPTH_ZERO_TO_ONE)
add_compile_definitions(GLM_ENABLE_EXPERIMENTAL)

include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/submodules/glfw/include
        ${CMAKE_SOURCE_DIR}/submodules/glm
        ${CMAKE_SOURCE_DIR}/submodules
        ${CMAKE_SOURCE_DIR}/middleware
        ${CMAKE_SOURCE_DIR}/middleware/freetype
        "C:/VulkanSDK/1.1.106.0/Include"
)

add_subdirectory(core)
add_subdirectory(render)
add_subdirectory(network)
add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(project_spaceships)
add_subdirectory(project_empty)

