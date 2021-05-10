#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    const Fixed Fixed::sMaxValue           = Fixed::FromData( ( ~0u ) >> 1 );
    const Fixed Fixed::sMinFractionalValue = Fixed::FromData( 1 );
    const Fixed Fixed::sMinValue           = Fixed::FromData( 1 << 31 );
    const Fixed Fixed::sTwoPi             = FX_TWO_PI;
    const Fixed Fixed::sPi                = FX_PI;
    const Fixed Fixed::sHalfPi            = FX_HALF_PI;
    const Fixed Fixed::sBias              = FX_BIAS;
    const Fixed Fixed::sFuzzyZero         = FX_FUZZY_ZERO;
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
}