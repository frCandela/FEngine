 set(EDITOR_NAME "editor_${GAME_NAME}")

 set(GAME_EDITOR_LIBS
         fanEditor
         ${GAME_LIBS}
         )

 if (${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC)
     # window executable with no console
     add_executable(${EDITOR_NAME} WIN32 ${GAME_SRC} ${GAME_H})
 else ()
     # default executable with a console
     add_executable(${EDITOR_NAME} ${GAME_SRC} ${GAME_H})
 endif ()

 target_link_libraries(${EDITOR_NAME} ${GAME_EDITOR_LIBS})
 target_compile_options(${EDITOR_NAME} PRIVATE ${COMPILATION_FLAGS})
 add_compile_definitions(FAN_EDITOR)
 set_target_properties(${EDITOR_NAME} PROPERTIES UNITY_BUILD ${FAN_ENABLE_UNITY_BUILD})

