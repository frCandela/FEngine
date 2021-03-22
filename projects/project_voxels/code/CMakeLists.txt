  set(PROJECT_NAME "voxels")

  file(GLOB_RECURSE PROJECT_SRC *.cpp)
  file(GLOB_RECURSE PROJECT_H *.hpp)

  set(PROJECT_LIBS
          fanEngine
          fanRender
          fanNetwork
          fanCore
          )

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  add_subdirectory(editor)
  add_subdirectory(game)