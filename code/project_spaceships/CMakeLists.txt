 set( PROJECT_NAME "spaceships")

 include_directories(
		 ${CMAKE_SOURCE_DIR}/code
 )

 file(GLOB_RECURSE PROJECT_SRC *.cpp)
 file(GLOB_RECURSE PROJECT_H *.hpp)

 set( PROJECT_LIBS
		 fanCore
		 fanRender
		 fanNetwork
		 fanEngine
		 )

 add_subdirectory(editor)
 add_subdirectory(game)