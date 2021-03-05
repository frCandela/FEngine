  set(PROJECT_NAME "empty")

  file(GLOB_RECURSE PROJECT_SRC *.cpp)
  file(GLOB_RECURSE PROJECT_H *.hpp)

  set(PROJECT_LIBS
          fanCore
          fanRender
          fanNetwork
          fanEngine
          )

  include_directories(${CMAKE_CURRENT_SOURCE_DIR})

  add_subdirectory(editor)
  add_subdirectory(game)