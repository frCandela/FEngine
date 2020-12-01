include_directories(
	${CMAKE_SOURCE_DIR}/code
)

file(GLOB_RECURSE FAN_PROJECT_SRC *.cpp)
file(GLOB_RECURSE FAN_PROJECT_H *.hpp)

set( PROJECT_NAME "project_spaceships")

set( FAN_PROJECT_LIBS
		 fanCore
		 fanRender
		 fanNetwork
		 fanEngine
		 fanEditor
		 )

if ( ${CMAKE_BUILD_TYPE} MATCHES Release AND MSVC  )
	 # window executable with no console
	 add_executable(${PROJECT_NAME} WIN32 ${FAN_PROJECT_SRC} ${FAN_PROJECT_H} )
else()
	 # default executable with a console
	 add_executable(${PROJECT_NAME} ${FAN_PROJECT_SRC} ${FAN_PROJECT_H} )
endif ()

target_link_libraries(${PROJECT_NAME} ${FAN_PROJECT_LIBS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILATION_FLAGS} )