#pragma once

// a=target variable, b=index
#define BIT_SET( a, b ) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR( a, b ) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP( a, b ) ((a) ^= (1ULL<<(b)))
#define BIT_TRUE( a, b ) (bool((a) & (1ULL<<(b))))
#define BIT_FALSE( a, b ) (!bool((a) & (1ULL<<(b))))

// x=target variable, y=mask
#define BITMASK_SET( x, y ) ((x) |= (y))
#define BITMASK_CLEAR( x, y ) ((x) &= (~(y)))
#define BITMASK_FLIP( x, y ) ((x) ^= (y))
#define BITMASK_TRUE_ALL( x, y ) (!(~(x) & (y)))
#define BITMASK_TRUE_ANY( x, y ) ((x) & (y))
