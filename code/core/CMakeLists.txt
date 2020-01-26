set(FAN_CORE_NAME fanCore)

include_directories(
	${CMAKE_SOURCE_DIR}/code
)

FILE(GLOB_RECURSE FAN_CORE_SRC *.cpp)
FILE(GLOB_RECURSE FAN_CORE_H *.hpp)

link_directories(lib/debug)

set(FAN_CORE_LIB
    glfw3
	LinearMath_Debug	
)

ADD_LIBRARY(${FAN_CORE_NAME} STATIC ${FAN_CORE_SRC} ${FAN_CORE_H})

target_link_libraries(${FAN_CORE_NAME} ${APP_LIBRARIES})