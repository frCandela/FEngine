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
#define WARNINGS_POP()		\
__pragma(warning( pop ))
#else

#define WARNINGS_BULLET_PUSH()
#define WARNINGS_GLM_PUSH()
#define WARNINGS_IMGUI_PUSH()
#define WARNINGS_POP()

#endif


