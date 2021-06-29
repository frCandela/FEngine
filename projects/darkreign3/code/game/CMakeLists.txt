 set(GAME_NAME "game_${GAME_NAME}")

 if (${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC)
     # window executable with no console
     add_executable(${GAME_NAME} WIN32 ${GAME_SRC} ${GAME_H})
 else ()
     # default executable with a console
     add_executable(${GAME_NAME} ${GAME_SRC} ${GAME_H})
 endif ()

 target_link_libraries(${GAME_NAME} ${GAME_LIBS})
 target_compile_options(${GAME_NAME} PRIVATE ${COMPILATION_FLAGS})
 set_target_properties(${GAME_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})

