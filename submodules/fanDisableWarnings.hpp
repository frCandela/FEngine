#pragma once 

#ifdef FAN_MSVC
//================================
//================================
#define WARNINGS_BULLET_PUSH()		\
__pragma(warning( push ))			\
__pragma(warning( disable : 4100 ))	\
__pragma(warning( disable : 4127 ))	

//================================
//================================
#define WARNINGS_GLM_PUSH()			\
__pragma(warning( push ))			\
__pragma(warning( disable : 4201 ))

//================================
//================================
#define WARNINGS_IMGUI_PUSH()		\
__pragma(warning( push ))			\
__pragma(warning( disable : 5054 ))

//================================
//================================
#define WARNINGS_LIVEPP()		        \
__pragma(warning( push ))			    \
__pragma(warning( disable : 4005 ))

//================================
//================================
#define WARNINGS_STB_PUSH()		\
__pragma(warning( push ))

//================================
//================================
#define WARNINGS_POP()		\
__pragma(warning( pop ))

//================================
//================================
#define WARNINGS_SFML_UTF_PUSH() \
__pragma(warning( push ))

#else

#define DO_PRAGMA(x) _Pragma (#x)

//================================
//================================
#define WARNINGS_BULLET_PUSH()
#define WARNINGS_GLM_PUSH()
#define WARNINGS_IMGUI_PUSH()

//================================
//================================
#define WARNINGS_LIVEPP()            \
DO_PRAGMA( GCC diagnostic push     )

//================================
//================================
#define WARNINGS_STB_PUSH()                         \
DO_PRAGMA( GCC diagnostic push     )                \
DO_PRAGMA( GCC diagnostic ignored "-Wsign-compare") \
DO_PRAGMA( GCC diagnostic ignored "-Wunused-but-set-variable" )

#define WARNINGS_SFML_UTF_PUSH()                    \
DO_PRAGMA( GCC diagnostic push     )                \
DO_PRAGMA( GCC diagnostic ignored "-Wimplicit-fallthrough")

//================================
//================================
#define WARNINGS_POP()		    \
DO_PRAGMA( GCC diagnostic pop)

#endif


