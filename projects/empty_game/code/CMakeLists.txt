  set(GAME_NAME "empty_game")

  file(GLOB_RECURSE GAME_SRC *.cpp)
  file(GLOB_RECURSE GAME_H *.hpp)

  set(GAME_LIBS
          fanCore
          fanRender
          fanNetwork
          fanEngine
          )

  include_directories(.)

  add_subdirectory(editor)
  add_subdirectory(game)