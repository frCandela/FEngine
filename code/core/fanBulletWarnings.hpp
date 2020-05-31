#pragma once 

//================================
//================================
#define BULLET_PUSH()					\
__pragma(warning( push ))				\
__pragma(warning( disable : 4100 ))	\
__pragma(warning( disable : 4127 ))	

//================================
//================================
#define BULLET_POP()		\
__pragma(warning( pop ))