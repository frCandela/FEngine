#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    const Fixed Fixed::sMaxValue           = Fixed::FromData( ( ~0u ) >> 1 );
    const Fixed Fixed::sMinFractionalValue = Fixed::FromData( 1 );
    const Fixed Fixed::sMinValue           = Fixed::FromData( 1 << 31 );
}