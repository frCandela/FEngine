  set(GAME_NAME "voxels")

  file(GLOB_RECURSE GAME_SRC *.cpp)
  file(GLOB_RECURSE GAME_H *.hpp)

  set(GAME_LIBS
          fanEngine
          fanRender
          fanNetwork
          fanCore
          )

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  add_subdirectory(editor)
  add_subdirectory(game)