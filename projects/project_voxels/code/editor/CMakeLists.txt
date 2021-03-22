 set( PROJECT_NAME_EDITOR  "project_${PROJECT_NAME}_editor")

 set( PROJECT_EDITOR_LIBS
         fanEditor
         ${PROJECT_LIBS}
 )

 if ( ${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC  )
     # window executable with no console
	 add_executable(${PROJECT_NAME_EDITOR} WIN32 ${PROJECT_SRC} ${PROJECT_H} )
 else()
     # default executable with a console
	 add_executable(${PROJECT_NAME_EDITOR} ${PROJECT_SRC} ${PROJECT_H} )
 endif ()

 target_link_libraries(${PROJECT_NAME_EDITOR} ${PROJECT_EDITOR_LIBS})
 target_compile_options(${PROJECT_NAME_EDITOR} PRIVATE ${COMPILATION_FLAGS} )
 add_compile_definitions(FAN_EDITOR)