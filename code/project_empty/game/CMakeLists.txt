 set( PROJECT_NAME_GAME   "project_${PROJECT_NAME}_game")

 if ( ${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC  )
     # window executable with no console
     add_executable(${PROJECT_NAME_GAME}  WIN32 ${PROJECT_SRC} ${PROJECT_H} )
 else()
     # default executable with a console
     add_executable(${PROJECT_NAME_GAME}  ${PROJECT_SRC} ${PROJECT_H} )
 endif ()

 target_link_libraries(${PROJECT_NAME_GAME} ${PROJECT_LIBS})
 target_compile_options(${PROJECT_NAME_GAME} PRIVATE ${COMPILATION_FLAGS} )