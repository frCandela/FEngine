cmake_minimum_required (VERSION 3.8)

project ("FEngine")
set(CMAKE_CXX_STANDARD 17)

include_directories( deps )
include_directories( code )
include_directories( "C:/VulkanSDK/1.1.106.0/Include" )

# link directories
link_directories("C:/VulkanSDK/1.1.106.0/Lib")
IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
    link_directories(lib/debug)
    link_directories("C:/VulkanSDK/1.1.106.0/Lib/debug")
ELSEIF(${CMAKE_BUILD_TYPE} MATCHES Release)
    link_directories(lib/release)
    link_directories("C:/VulkanSDK/1.1.106.0/Lib/release")
ENDIF()

# libs directories
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

add_executable(fengine_editor ${APP_SOURCES} )

target_link_libraries(fengine_editor ${APP_LIBRARIES})

# FORCE C++17 STANDARD
set_target_properties(fengine_editor PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

  # Force W4
if(MSVC)
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  endif()
endif()

  # Force WX (warning as error)
target_compile_options(fanCore PRIVATE /WX /INCREMENTAL /ZI) 
target_compile_options(fanRender PRIVATE /WX /INCREMENTAL /ZI)
target_compile_options(fanNetwork PRIVATE /WX /INCREMENTAL/ZI )
target_compile_options(fanScene PRIVATE /WX /INCREMENTAL /ZI)
target_compile_options(fanGame PRIVATE /WX /INCREMENTAL /ZI)
target_compile_options(fanEditor PRIVATE /WX /INCREMENTAL /ZI)
target_compile_options(fengine_editor PRIVATE /WX /INCREMENTAL /ZI)