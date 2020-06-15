#pragma once 

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
#define WARNINGS_POP()		\
__pragma(warning( pop ))


