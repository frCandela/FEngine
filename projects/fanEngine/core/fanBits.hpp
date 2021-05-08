#pragma once

#define BIT_SET( a, b ) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR( a, b ) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP( a, b ) ((a) ^= (1ULL<<(b)))
#define BIT_IS_TRUE( a, b ) (bool((a) & (1ULL<<(b))))
#define BIT_IS_FALSE( a, b ) (!bool((a) & (1ULL<<(b))))

// commented code is untested , please unit test it before using it
/*
// x=target variable, y=mask
#define BITMASK_SET(x,y) ((x) |= (y))
#define BITMASK_CLEAR(x,y) ((x) &= (~(y)))
#define BITMASK_FLIP(x,y) ((x) ^= (y))
#define BITMASK_CHECK_ALL(x,y) (!(~(x) & (y)))
#define BITMASK_CHECK_ANY(x,y) ((x) & (y))
 */