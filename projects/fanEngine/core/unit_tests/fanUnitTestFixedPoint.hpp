#pragma once

#include <core/time/fanScopedTimer.hpp>
#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestFixedPoint : public UnitTest<UnitTestFixedPoint>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestFixedPoint::TestIntegers,       "Integer constructor" },
                     { &UnitTestFixedPoint::TestFloats,         "Float constructor" },
                     { &UnitTestFixedPoint::TestDoubles,        "Double constructor" },
                     { &UnitTestFixedPoint::TestStrings,        "String constructor" },
                     { &UnitTestFixedPoint::TestComparisons,    "Comparisons" },
                     { &UnitTestFixedPoint::TestSpecialValues,  "Special values" },
                     { &UnitTestFixedPoint::TestAddition,       "Addition" },
                     { &UnitTestFixedPoint::TestSubtraction,    "Subtraction" },
                     { &UnitTestFixedPoint::TestMultiplication, "Addition" },
                     { &UnitTestFixedPoint::TestDivision,       "Division" },
                     { &UnitTestFixedPoint::TestModulo,         "Modulo" },
                     { &UnitTestFixedPoint::TestRadians,        "radians conversion" },
                     { &UnitTestFixedPoint::TestSign,           "Sign" },
                     { &UnitTestFixedPoint::TestFloor,          "Floor" },
                     { &UnitTestFixedPoint::TestCeil,           "Ceil" },
                     { &UnitTestFixedPoint::TestRound,          "Round" },
                     { &UnitTestFixedPoint::TestAbs,            "Abs" },
                     { &UnitTestFixedPoint::TestMin,            "Min" },
                     { &UnitTestFixedPoint::TestMax,            "Max" },
                     { &UnitTestFixedPoint::TestPowI,           "PowI" },
                     { &UnitTestFixedPoint::TestSin,            "Sin" },
                     { &UnitTestFixedPoint::TestASin,           "ASin" },
                     { &UnitTestFixedPoint::TestCos,            "Cos" },
                     { &UnitTestFixedPoint::TestACos,           "ACos" },
                     { &UnitTestFixedPoint::TestTan,            "Tan" },
                     { &UnitTestFixedPoint::TestATan2,          "ATan2" },
                     { &UnitTestFixedPoint::TestSqrt,           "Sqrt" },
                     { &UnitTestFixedPoint::TestInvSqrt,        "InvSqrt" },
            };
        }

        void Create() override
        {
        }

        void Destroy() override
        {
        }

        // test two functions over an interval with a step size, return the maximal error
        using FixedFunction = Fixed( * )( const Fixed& );
        using DoubleFunction = double ( * )( double );
        double MaxErrorFixedVsDouble( FixedFunction _fixedFunction,
                                      DoubleFunction _doubleFunction,
                                      double _start,
                                      double _end,
                                      double _step )
        {
            double      maxError = 0;
            for( double i        = _start; i < _end; i += _step )
            {
                double fixedResult  = ( *_fixedFunction )( Fixed::FromDouble( i ) ).ToDouble();
                double doubleResult = ( *_doubleFunction )( i );
                double diff         = std::abs( fixedResult - doubleResult );
                maxError = std::max( maxError, diff );
            }
            return maxError;
        }

        void TestIntegers()
        {
            TEST_ASSERT( Fixed( 4 ).ToInt() == 4 );
            TEST_ASSERT( Fixed( -4 ).ToInt() == -4 );
            TEST_ASSERT( Fixed( Fixed::sMaxInteger ).ToInt() == Fixed::sMaxInteger );
            TEST_ASSERT( Fixed( Fixed::sMaxInteger + 1 ).ToInt() != Fixed::sMaxInteger + 1 );
            TEST_ASSERT( Fixed( Fixed::sMinInteger ).ToInt() == Fixed::sMinInteger );
            TEST_ASSERT( Fixed( Fixed::sMinInteger - 1 ).ToInt() != Fixed::sMinInteger - 1 );
        }

        void TestFloats()
        {
            TEST_ASSERT( Fixed::FromFloat( 5.0625f ).ToFloat() == 5.0625f );
            TEST_ASSERT( Fixed::FromFloat( -5.0625f ).ToFloat() == -5.0625f );
            TEST_ASSERT( Fixed::FromFloat( float( Fixed::sMaxInteger ) ).ToFloat() == Fixed::sMaxInteger );
            TEST_ASSERT( Fixed::FromFloat( float( Fixed::sMax ) + 1.f ).ToFloat() != Fixed::sMax + 1.f );
            TEST_ASSERT( Fixed::FromFloat( float( Fixed::sMin ) ).ToFloat() == Fixed::sMin );
            TEST_ASSERT( Fixed::FromFloat( float( Fixed::sMin ) - 1.f ).ToFloat() != Fixed::sMin - 1.f );
        }

        void TestDoubles()
        {
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMinFractional ).ToDouble() == Fixed::sMinFractional );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMinFractional / 2. ).ToDouble() != Fixed::sMinFractional / 2. );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMaxFractional ).ToDouble() == Fixed::sMaxFractional );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMaxFractional + Fixed::sMinFractional ).ToDouble() == 1. );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMax ).ToDouble() == Fixed::sMax );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMin ).ToDouble() == Fixed::sMin );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMax + Fixed::sMinFractional ).ToDouble() !=
                         Fixed::sMax + Fixed::sMinFractional );
            TEST_ASSERT( Fixed::FromDouble( Fixed::sMin - Fixed::sMinFractional ).ToDouble() !=
                         Fixed::sMin - Fixed::sMinFractional );
        }

        void TestStrings()
        {
            // positives
            TEST_ASSERT( Fixed( "123" ).ToFloat() == 123.f );
            TEST_ASSERT( Fixed( ".75" ).ToFloat() == .75f );
            TEST_ASSERT( Fixed( "0000.0625" ).ToFloat() == .0625f );
            TEST_ASSERT( Fixed( "0.625" ).ToFloat() == 0.625f );

            // negatives
            TEST_ASSERT( FIXED( -123 ).ToFloat() == -123.f );
            TEST_ASSERT( FIXED( -.75 ).ToFloat() == -.75f );
            TEST_ASSERT( FIXED( -0000.0625 ).ToFloat() == -.0625f );
            TEST_ASSERT( FIXED( -25.625 ).ToFloat() == -25.625f );

            // clamp max digits
            TEST_ASSERT( ( FIXED( 0.999999999999999999 ) ).ToDouble() == FIXED( 0.999999999 ).ToDouble() );

            // limits
            TEST_ASSERT( Fixed( std::to_string( Fixed::sMax ).c_str() ).ToDouble() == Fixed::sMax );
            TEST_ASSERT( Fixed( std::to_string( Fixed::sMin ).c_str() ).ToDouble() == Fixed::sMin );
        }

        void TestComparisons()
        {
            static_assert( FIXED( 1 ) < FIXED( 2 ) );

            TEST_ASSERT( FIXED( 2.5 ) > FIXED( 1.5 ) )
            TEST_ASSERT( FIXED( 2.5 ) >= FIXED( 1.5 ) )
            TEST_ASSERT( FIXED( -2.5 ) < FIXED( -1.5 ) )
            TEST_ASSERT( FIXED( -2.5 ) <= FIXED( -1.5 ) )
            TEST_ASSERT( FIXED( 2.5 ) >= FIXED( 2.5 ) )
            TEST_ASSERT( FIXED( -2.5 ) <= FIXED( -2.5 ) )
            TEST_ASSERT( FIXED( -2.5 ) == FIXED( -2.5 ) )
            TEST_ASSERT( !( FIXED( -2.5 ) == FIXED( -2.6 ) ) )
            TEST_ASSERT( FIXED( -2.5 ) != FIXED( -2.6 ) )
            TEST_ASSERT( !( FIXED( -2.5 ) != FIXED( -2.5 ) ) )
        }

        void TestSpecialValues()
        {
            Fixed maxPlusOne = Fixed::FromData( Fixed::sMaxValue.GetData() + 1 );
            TEST_ASSERT( maxPlusOne < 0 )
            TEST_ASSERT( Fixed::sMinFractionalValue / 2 == 0 )
            TEST_ASSERT( Fixed::sMaxValue + Fixed::sMinFractionalValue < 0 )
            TEST_ASSERT( Fixed::sMinValue - Fixed::sMinFractionalValue > 0 )
        }

        void TestAddition()
        {
            static_assert( FIXED( 1 ) + FIXED( 2 ) == FIXED( 3 ) );

            // (+)
            TEST_ASSERT( FIXED( 1.5 ) + FIXED( 2.25 ) == FIXED( 3.75 ) );
            TEST_ASSERT( FIXED( 2.25 ) + FIXED( 1.5 ) == FIXED( 3.75 ) );
            TEST_ASSERT( FIXED( 2.25 ) + FIXED( 0 ) == FIXED( 2.25 ) );
            TEST_ASSERT( FIXED( -3.5 ) + FIXED( -4.125 ) == FIXED( -7.625 ) );
            TEST_ASSERT( FIXED( -4.125 ) + FIXED( -3.5 ) == FIXED( -7.625 ) );
            TEST_ASSERT( FIXED( -4.125 ) + FIXED( 0 ) == FIXED( -4.125 ) );
            TEST_ASSERT( FIXED( 3.5 ) + FIXED( -3.5 ) == FIXED( 0 ) );
            // (+=)
            TEST_ASSERT( ( FIXED( 1.5 ) += FIXED( 2.25 ) ) == FIXED( 3.75 ) );
            TEST_ASSERT( ( FIXED( -3.5 ) += FIXED( -4.125 ) ) == FIXED( -7.625 ) );
            // ++
            Fixed g = FIXED( 1.5 );
            TEST_ASSERT( ++g == FIXED( 2.5 ) );
            TEST_ASSERT( g++ == FIXED( 2.5 ) );
            TEST_ASSERT( g == FIXED( 3.5 ) );

            // integer addition
            TEST_ASSERT( 2 + FIXED( 3.5 ) == FIXED( 5.5 ) );
            TEST_ASSERT( -3 + FIXED( 3.5 ) == FIXED( 0.5 ) );
        }

        void TestSubtraction()
        {
            static_assert( FIXED( 1 ) - FIXED( 3 ) == FIXED( -2 ) );

            // (-)
            TEST_ASSERT( FIXED( 4.5 ) - FIXED( 1.25 ) == FIXED( 3.25 ) );
            TEST_ASSERT( FIXED( 1.25 ) - FIXED( 4.5 ) == FIXED( -3.25 ) );
            TEST_ASSERT( FIXED( 1.25 ) - FIXED( 0 ) == FIXED( 1.25 ) );
            TEST_ASSERT( FIXED( -1.25 ) - FIXED( 2.5 ) == FIXED( -3.75 ) );
            TEST_ASSERT( 3 - FIXED( 1 ) == 2 )

            // (-=)
            TEST_ASSERT( ( FIXED( 4.5 ) -= FIXED( 1.25 ) ) == FIXED( 3.25 ) );
            TEST_ASSERT( ( FIXED( -12.5 ) -= FIXED( 1.25 ) ) == FIXED( -13.75 ) );
            // --
            Fixed g = FIXED( 1.5 );
            TEST_ASSERT( --g == FIXED( 0.5 ) );
            TEST_ASSERT( g-- == FIXED( 0.5 ) );
            TEST_ASSERT( g == FIXED( -0.5 ) );

            TEST_ASSERT( ( 1 - FIXED( 2.5 ) ) == FIXED( -1.5 ) );
        }

        void TestMultiplication()
        {
            static_assert( FIXED( 2 ) * FIXED( 3 ) == FIXED( 6 ) );

            // (*)
            TEST_ASSERT( FIXED( 2 ) * FIXED( 3 ) == FIXED( 6 ) );
            TEST_ASSERT( FIXED( -2 ) * FIXED( 3 ) == FIXED( -6 ) );
            TEST_ASSERT( FIXED( -2 ) * FIXED( -3 ) == FIXED( 6 ) );
            TEST_ASSERT( FIXED( 0.5 ) * FIXED( 3 ) == FIXED( 1.5 ) );
            TEST_ASSERT( FIXED( 2 ) * FIXED( -3.5 ) == FIXED( -7 ) );

            // (*=)
            TEST_ASSERT( ( FIXED( 4.5 ) *= FIXED( 2.5 ) ) == FIXED( 11.25 ) );
            TEST_ASSERT( ( FIXED( 4.5 ) *= FIXED( -2.5 ) ) == FIXED( -11.25 ) );
            TEST_ASSERT( ( FIXED( -4.5 ) *= FIXED( 2.5 ) ) == FIXED( -11.25 ) );

            TEST_ASSERT( ( 2 * FIXED( 8.5 ) ) == FIXED( 17 ) );
        }

        void TestDivision()
        {
            static_assert( FIXED( 6 ) / FIXED( 2 ) == FIXED( 3 ) );

            // (/)
            TEST_ASSERT( FIXED( 6 ) / FIXED( 2 ) == FIXED( 3 ) );
            TEST_ASSERT( FIXED( -6 ) / FIXED( 2 ) == -FIXED( 3 ) );
            TEST_ASSERT( FIXED( 1 ) / FIXED( 2 ) == FIXED( .5 ) );
            TEST_ASSERT( FIXED( 1 ) / FIXED( -2 ) == FIXED( -.5 ) );

            // (/=)
            TEST_ASSERT( ( FIXED( 8 ) /= FIXED( 2 ) ) == FIXED( 4. ) );
            TEST_ASSERT( ( FIXED( 4.5 ) /= FIXED( -2 ) ) == FIXED( -2.25 ) );

            TEST_ASSERT( 6 / FIXED( 2 ) == FIXED( 3 ) );
        }

        void TestModulo()
        {
            static_assert( FIXED( 3 ) % FIXED( 2 ) == FIXED( 1 ) );

            // (%)
            TEST_ASSERT( FIXED( 7 ) % FIXED( 2 ) == FIXED( 1 ) );
            TEST_ASSERT( FIXED( -7 ) % FIXED( 2 ) == FIXED( -1 ) );
            TEST_ASSERT( FIXED( 7.5 ) % FIXED( 2 ) == FIXED( 1.5 ) );
            TEST_ASSERT( FIXED( -7.5 ) % FIXED( 2 ) == FIXED( -1.5 ) );
            TEST_ASSERT( FIXED( 7.5 ) % FIXED( -2 ) == FIXED( 1.5 ) );
            TEST_ASSERT( FIXED( -7.5 ) % FIXED( -2 ) == FIXED( -1.5 ) );

            TEST_ASSERT( FIXED( 6.28 ) % FIXED( 3.14 ) == FIXED( 0 ) );
            TEST_ASSERT( FIXED( -6.28 ) % FIXED( 3.14 ) == FIXED( 0 ) );
            TEST_ASSERT( FIXED( 4.14 ) % FIXED( 3.14 ) == FIXED( 1 ) );
            TEST_ASSERT( FIXED( -4.14 ) % FIXED( 3.14 ) == FIXED( -1 ) );

            // (%=)
            TEST_ASSERT( ( FIXED( 8.5 ) %= FIXED( 2 ) ) == FIXED( 0.5 ) );
            TEST_ASSERT( ( FIXED( -8.5 ) %= FIXED( -2 ) ) == FIXED( -0.5 ) );
        }

        void TestRadians()
        {
            static_assert( Fixed::Radians( 0 ) == FIXED( 0 ) );
            static_assert( Fixed::Degrees( 0 ) == FIXED( 0 ) );


            // Radians
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( 90 ) - Fixed::sHalfPi ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( 180 ) - Fixed::sPi ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( -180 ) - ( -Fixed::sPi ) ) );

            double d = ( Fixed::Degrees( Fixed::sHalfPi ) - 90 ).ToDouble();
            (void)d;

            // Radians
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( Fixed::sHalfPi ) - 90 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( Fixed::sPi ) - 180 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( -Fixed::sPi ) - ( -180 ) ) );
        }

        void TestSign()
        {
            static_assert( Fixed::Sign( 1 ) == FIXED( 1 ) );
            TEST_ASSERT( Fixed::Sign( -1 ) == FIXED( -1 ) );
            TEST_ASSERT( Fixed::Sign( 0 ) == FIXED( 1 ) );
        }

        void TestFloor()
        {
            static_assert( Fixed::Floor( FIXED( 1.6 ) ) == FIXED( 1 ) );

            TEST_ASSERT( Fixed::Floor( FIXED( 1.5 ) ) == FIXED( 1 ) );
            TEST_ASSERT( Fixed::Floor( FIXED( -1.5 ) ) == FIXED( -2 ) );
            TEST_ASSERT( Fixed::Floor( FIXED( 3 ) ) == FIXED( 3 ) );
            TEST_ASSERT( Fixed::Floor( FIXED( -3 ) ) == FIXED( -3 ) );
        }

        void TestCeil()
        {
            static_assert( Fixed::Ceil( FIXED( 0.4 ) ) == FIXED( 1 ) );

            TEST_ASSERT( Fixed::Ceil( FIXED( 1.5 ) ) == FIXED( 2 ) );
            TEST_ASSERT( Fixed::Ceil( FIXED( -1.5 ) ) == FIXED( -1 ) );
            TEST_ASSERT( Fixed::Ceil( FIXED( 3 ) ) == FIXED( 3 ) );
            TEST_ASSERT( Fixed::Ceil( FIXED( -3 ) ) == FIXED( -3 ) );
        }

        void TestRound()
        {
            static_assert( Fixed::Round( FIXED( 1.6 ) ) == FIXED( 2 ) );

            TEST_ASSERT( Fixed::Round( FIXED( 1.6 ) ) == FIXED( 2 ) );
            TEST_ASSERT( Fixed::Round( FIXED( 1.4 ) ) == FIXED( 1 ) );
            TEST_ASSERT( Fixed::Round( FIXED( -1.6 ) ) == FIXED( -2 ) );
            TEST_ASSERT( Fixed::Round( FIXED( -1.4 ) ) == FIXED( -1 ) );

            TEST_ASSERT( Fixed::Round( FIXED( 1.5 ) ) == FIXED( 2 ) );
            TEST_ASSERT( Fixed::Round( FIXED( -1.5 ) ) == FIXED( -1 ) );
        }

        void TestAbs()
        {
            static_assert( Fixed::Abs( FIXED( -1 ) ) == FIXED( 1 ) );

            TEST_ASSERT( Fixed::Abs( FIXED( 1.5 ) ) == FIXED( 1.5 ) )
            TEST_ASSERT( Fixed::Abs( FIXED( -1.5 ) ) == FIXED( 1.5 ) )
        }

        void TestMin()
        {
            static_assert( Fixed::Min( 1, 2 ) == 1 );

            TEST_ASSERT( Fixed::Min( 0, 2 ) == 0 )
            TEST_ASSERT( Fixed::Min( 2, 0 ) == 0 )
            TEST_ASSERT( Fixed::Min( -2, 0 ) == -2 )
            TEST_ASSERT( Fixed::Min( 0, -2 ) == -2 )
            TEST_ASSERT( Fixed::Min( 0, 0 ) == 0 )
        }

        void TestMax()
        {
            static_assert( Fixed::Max( 1, 2 ) == 2 );

            TEST_ASSERT( Fixed::Max( 0, 2 ) == 2 )
            TEST_ASSERT( Fixed::Max( 2, 0 ) == 2 )
            TEST_ASSERT( Fixed::Max( -2, 0 ) == 0 )
            TEST_ASSERT( Fixed::Max( 0, -2 ) == 0 )
            TEST_ASSERT( Fixed::Max( 0, 0 ) == 0 )
        }

        void TestPowI()
        {
            static_assert( Fixed::PowI( FIXED( 4 ), 2 ) == FIXED( 16 ) );

            TEST_ASSERT( Fixed::PowI( FIXED( 1.5 ), 0 ) == FIXED( 1 ) );
            TEST_ASSERT( Fixed::PowI( FIXED( 1.5 ), 2 ) == FIXED( 2.25 ) );
            TEST_ASSERT( Fixed::PowI( FIXED( 2 ), 2 ) == FIXED( 4 ) );
            TEST_ASSERT( Fixed::PowI( FIXED( -2 ), 3 ) == FIXED( -8 ) );
            TEST_ASSERT( Fixed::PowI( FIXED( 3.5 ), 3 ) == FIXED( 42.875 ) );
        }

        void TestSin()
        {
            static_assert( Fixed::Sin( FIXED( 0 ) ) == FIXED( 0 ) );

            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::sPi - FIXED( 3.141592654 ) ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( Fixed::sPi ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( Fixed::sHalfPi ) - 1 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( -Fixed::sHalfPi ) - ( -1 ) ) );
            TEST_ASSERT( Fixed::Sin( Fixed::sPi + 1 ) == Fixed::Sin( -Fixed::sPi + 1 ) );
            TEST_ASSERT( Fixed::Sin( -Fixed::sPi - 1 ) == Fixed::Sin( Fixed::sPi - 1 ) );

            FixedFunction  fxSin     = &Fixed::Sin;
            DoubleFunction doubleSin = &std::sin;
            double         error     = MaxErrorFixedVsDouble( fxSin,
                                                              doubleSin,
                                                              ( -Fixed::sTwoPi ).ToDouble(),
                                                              ( Fixed::sTwoPi ).ToDouble(),
                                                              0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [0,1]
        }

        void TestASin()
        {
            static_assert( Fixed::ASin( 1 ) == FX_HALF_PI );

            FixedFunction  fxASin     = &Fixed::ASin;
            DoubleFunction doubleASin = &std::asin;
            double         error      = MaxErrorFixedVsDouble( fxASin, doubleASin, -0.99, 0.99, 0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [-1,1]
        }

        void TestCos()
        {
            static_assert( Fixed::Cos( 0 ) == 1 );

            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( Fixed::sPi ) - ( -1 ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( Fixed::sHalfPi ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( -Fixed::sHalfPi ) ) );

            TEST_ASSERT( Fixed::Cos( Fixed::sPi + 1 ) == Fixed::Cos( -Fixed::sPi + 1 ) );
            TEST_ASSERT( Fixed::Cos( -Fixed::sPi - 1 ) == Fixed::Cos( Fixed::sPi - 1 ) );

            FixedFunction  fxCos     = &Fixed::Cos;
            DoubleFunction doubleCos = &std::cos;
            double         error     = MaxErrorFixedVsDouble( fxCos, doubleCos, ( -Fixed::sTwoPi ).ToDouble(), ( Fixed::sTwoPi ).ToDouble(), 0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [0,1]
        }

        void TestACos()
        {
            static_assert( Fixed::ACos( FIXED( 1 ) ) == 0 );

            FixedFunction  fxACos     = &Fixed::ACos;
            DoubleFunction doubleACos = &std::acos;
            double         error      = MaxErrorFixedVsDouble( fxACos, doubleACos, -0.99, 0.99, 0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [-1,1]
        }

        void TestTan()
        {
            static_assert( Fixed::Tan( 0 ) == 0 );

            FixedFunction  fxTan     = &Fixed::Tan;
            DoubleFunction doubleTan = &std::tan;
            const double   step      = 0.001;
            double         error     = MaxErrorFixedVsDouble( fxTan, doubleTan, ( -Fixed::sHalfPi ).ToDouble() + 0.1, ( Fixed::sHalfPi ).ToDouble() - 0.1, step );
            TEST_ASSERT( error < 0.01 )
        }

        void TestATan2()
        {
            static_assert( Fixed::ATan2( 0, 1 ) == 0 );

            // test the error over the trigonometric circle
            const double step     = 0.001;
            double       maxError = 0;
            for( double  angle    = 0; angle < glm::pi<double>(); angle += step )
            {
                const double x = std::cos( angle );
                const double y = std::sin( angle );

                double fixedResult  = Fixed::ATan2( Fixed::FromDouble( y ), Fixed::FromDouble( x ) ).ToDouble();
                double doubleResult = std::atan2( y, x );
                double diff         = std::abs( fixedResult - doubleResult );
                maxError = std::max( maxError, diff );
            }

            TEST_ASSERT( maxError < 0.0005 )
        }

        void TestSqrt()
        {
            static_assert( Fixed::Sqrt( 4 ) == 2 );

            TEST_ASSERT( Fixed::Sqrt( 0 ) == 0 )
            TEST_ASSERT( Fixed::Sqrt( FIXED( 1.75 ) * FIXED( 1.75 ) ) == FIXED( 1.75 ) )

            FixedFunction  fxSqrt     = &Fixed::Sqrt;
            DoubleFunction doubleSqrt = &std::sqrt;
            double         error      = MaxErrorFixedVsDouble( fxSqrt, doubleSqrt, 0, 1., 0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [0,1]
            error = MaxErrorFixedVsDouble( fxSqrt, doubleSqrt, 1., Fixed::sMaxInteger, 0.1 );
            TEST_ASSERT( error < 0.0005 )// [1,max]
        }

        static double sqrtRef( double d ) { return 1. / std::sqrt( d ); }
        void TestInvSqrt()
        {
            {
                //ScopedTimer timer( "fast" );
                double error = MaxErrorFixedVsDouble( &Fixed::InvSqrt, &sqrtRef, 0.05, 1, 0.001 );
                TEST_ASSERT( error < 0.01 )// [1,max]
                error = MaxErrorFixedVsDouble( &Fixed::InvSqrt, &sqrtRef, 1, 1000, 0.1 );
                TEST_ASSERT( error < 0.01 )// [1,max]
            }
            /* {
                // ScopedTimer timer( "slow" );
                 double      error = MaxErrorFixedVsDouble( &SlowInvSqrt, &sqrtRef, 0.05, 1, 0.001 );
                 TEST_ASSERT( error < 0.01 )// [1,max]
                 error             = MaxErrorFixedVsDouble( &SlowInvSqrt, &sqrtRef, 1, 1000, 0.01 );
                 TEST_ASSERT( error < 0.01 )// [1,max]
             }*/
        }
    };
}