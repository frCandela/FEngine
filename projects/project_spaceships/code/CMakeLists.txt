 set( PROJECT_NAME "spaceships")

 file(GLOB_RECURSE PROJECT_SRC *.cpp)
 file(GLOB_RECURSE PROJECT_H *.hpp)

 set( PROJECT_LIBS
		 fanCore
		 fanRender
		 fanNetwork
		 fanEngine
		 )

 include_directories(${CMAKE_CURRENT_SOURCE_DIR})
 add_compile_definitions(PROJECT_PATH="${CMAKE_CURRENT_SOURCE_DIR}")
 add_subdirectory(editor)
 add_subdirectory(game)