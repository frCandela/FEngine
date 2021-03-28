#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanFixedPoint.hpp"

namespace fan
{
    //======================================================================================================================
    //======================================================================================================================
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
                     { &UnitTestFixedPoint::TestATan2,          "ATan2" },
                     { &UnitTestFixedPoint::TestSqrt,           "Sqrt" },
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
            TEST_ASSERT( ( 123_fx ).ToFloat() == 123.f );
            TEST_ASSERT( ( .75_fx ).ToFloat() == .75f );
            TEST_ASSERT( ( 0000.0625_fx ).ToFloat() == .0625f );
            TEST_ASSERT( ( 0.625_fx ).ToFloat() == 0.625f );

            // negatives
            TEST_ASSERT( Fixed( "-123" ).ToFloat() == -123.f );
            TEST_ASSERT( Fixed( "-.75" ).ToFloat() == -.75f );
            TEST_ASSERT( Fixed( "-0000.0625" ).ToFloat() == -.0625f );
            TEST_ASSERT( Fixed( "-25.625" ).ToFloat() == -25.625f );

            // clamp max digits
            TEST_ASSERT( ( 0.999999999999999999_fx ).ToDouble() == ( 0.999999999_fx ).ToDouble() );

            // limits
            TEST_ASSERT( Fixed( std::to_string( Fixed::sMax ).c_str() ).ToDouble() == Fixed::sMax );
            TEST_ASSERT( Fixed( std::to_string( Fixed::sMin ).c_str() ).ToDouble() == Fixed::sMin );
        }

        void TestComparisons()
        {
            static_assert( 1_fx < 2_fx );

            TEST_ASSERT( 2.5_fx > 1.5_fx )
            TEST_ASSERT( 2.5_fx >= 1.5_fx )
            TEST_ASSERT( -2.5_fx < -1.5_fx )
            TEST_ASSERT( -2.5_fx <= -1.5_fx )
            TEST_ASSERT( 2.5_fx >= 2.5_fx )
            TEST_ASSERT( -2.5_fx <= -2.5_fx )
            TEST_ASSERT( -2.5_fx == -2.5_fx )
            TEST_ASSERT( !( -2.5_fx == -2.6_fx ) )
            TEST_ASSERT( -2.5_fx != -2.6_fx )
            TEST_ASSERT( !( -2.5_fx != -2.5_fx ) )
        }

        void TestAddition()
        {
            static_assert( 1_fx + 2_fx == 3_fx );

            // (+)
            TEST_ASSERT( 1.5_fx + 2.25_fx == 3.75_fx );
            TEST_ASSERT( 2.25_fx + 1.5_fx == 3.75_fx );
            TEST_ASSERT( 2.25_fx + 0_fx == 2.25_fx );
            TEST_ASSERT( -3.5_fx + -4.125_fx == -7.625_fx );
            TEST_ASSERT( -4.125_fx + -3.5_fx == -7.625_fx );
            TEST_ASSERT( -4.125_fx + 0_fx == -4.125_fx );
            TEST_ASSERT( 3.5_fx + -3.5_fx == 0_fx );
            // (+=)
            TEST_ASSERT( ( 1.5_fx += 2.25_fx ) == 3.75_fx );
            TEST_ASSERT( ( -3.5_fx += -4.125_fx ) == -7.625_fx );
            // ++
            Fixed g = 1.5_fx;
            TEST_ASSERT( ++g == 2.5_fx );
            TEST_ASSERT( g++ == 2.5_fx );
            TEST_ASSERT( g == 3.5_fx );
        }

        void TestSubtraction()
        {
            static_assert( 1_fx - 3_fx == -2_fx );

            // (-)
            TEST_ASSERT( 4.5_fx - 1.25_fx == 3.25_fx );
            TEST_ASSERT( 1.25_fx - 4.5_fx == -3.25_fx );
            TEST_ASSERT( 1.25_fx - 0_fx == 1.25_fx );
            TEST_ASSERT( -1.25_fx - 2.5_fx == -3.75_fx );
            // (-=)
            TEST_ASSERT( ( 4.5_fx -= 1.25_fx ) == 3.25_fx );
            TEST_ASSERT( ( -12.5_fx -= 1.25_fx ) == -13.75_fx );
            // --
            Fixed g = 1.5_fx;
            TEST_ASSERT( --g == 0.5_fx );
            TEST_ASSERT( g-- == 0.5_fx );
            TEST_ASSERT( g == -0.5_fx );
        }

        void TestMultiplication()
        {
            static_assert( 2_fx * 3_fx == 6_fx );

            // (*)
            TEST_ASSERT( 2_fx * 3_fx == 6_fx );
            TEST_ASSERT( -2_fx * 3_fx == -6_fx );
            TEST_ASSERT( -2_fx * -3_fx == 6_fx );
            TEST_ASSERT( 0.5_fx * 3_fx == 1.5_fx );
            TEST_ASSERT( 2_fx * -3.5_fx == -7_fx );

            // (*=)
            TEST_ASSERT( ( 4.5_fx *= 2.5_fx ) == 11.25_fx );
            TEST_ASSERT( ( 4.5_fx *= -2.5_fx ) == -11.25_fx );
            TEST_ASSERT( ( -4.5_fx *= 2.5_fx ) == -11.25_fx );
        }

        void TestDivision()
        {
            static_assert( 6_fx / 2_fx == 3_fx );

            // (/)
            TEST_ASSERT( 6_fx / 2_fx == 3_fx );
            TEST_ASSERT( -6_fx / 2_fx == -3_fx );
            TEST_ASSERT( 1_fx / 2_fx == .5_fx );
            TEST_ASSERT( 1_fx / -2_fx == -.5_fx );

            // (/=)
            TEST_ASSERT( ( 8_fx /= 2_fx ) == 4._fx );
            TEST_ASSERT( ( 4.5_fx /= -2_fx ) == -2.25_fx );
        }

        void TestModulo()
        {
            static_assert( 3_fx % 2_fx == 1_fx );

            // (%)
            TEST_ASSERT( 7_fx % 2_fx == 1_fx );
            TEST_ASSERT( -7_fx % 2_fx == -1_fx );
            TEST_ASSERT( 7.5_fx % 2_fx == 1.5_fx );
            TEST_ASSERT( -7.5_fx % 2_fx == -1.5_fx );
            TEST_ASSERT( 7.5_fx % -2_fx == 1.5_fx );
            TEST_ASSERT( -7.5_fx % -2_fx == -1.5_fx );

            TEST_ASSERT( 6.28_fx % 3.14_fx == 0_fx );
            TEST_ASSERT( -6.28_fx % 3.14_fx == 0_fx );
            TEST_ASSERT( 4.14_fx % 3.14_fx == 1_fx );
            TEST_ASSERT( -4.14_fx % 3.14_fx == -1_fx );

            // (%=)
            TEST_ASSERT( ( 8.5_fx %= 2_fx ) == 0.5_fx );
            TEST_ASSERT( ( -8.5_fx %= -2_fx ) == -0.5_fx );
        }

        void TestRadians()
        {
            static_assert( Fixed::Radians( 0 ) == FIXED( 0 ) );
            static_assert( Fixed::Degrees( 0 ) == FIXED( 0 ) );


            // Radians
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( 90 ) - FX_HALF_PI ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( 180 ) - FX_PI ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Radians( -180 ) - ( -FX_PI ) ) );

            double d = ( Fixed::Degrees( FX_HALF_PI ) - 90 ).ToDouble();
            (void)d;

            // Radians
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( FX_HALF_PI ) - 90 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( FX_PI ) - 180 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Degrees( -FX_PI ) - ( -180 ) ) );
        }

        void TestSign()
        {
            static_assert( Fixed::Sign( 1 ) == FIXED( 1 ) );
            TEST_ASSERT( Fixed::Sign( -1 ) == -1_fx );
            TEST_ASSERT( Fixed::Sign( 0 ) == 1_fx );
        }

        void TestFloor()
        {
            static_assert( Fixed::Floor( 1.6_fx ) == 1_fx );

            TEST_ASSERT( Fixed::Floor( 1.5_fx ) == 1_fx );
            TEST_ASSERT( Fixed::Floor( -1.5_fx ) == -2_fx );
            TEST_ASSERT( Fixed::Floor( 3_fx ) == 3_fx );
            TEST_ASSERT( Fixed::Floor( -3_fx ) == -3_fx );
        }

        void TestCeil()
        {
            static_assert( Fixed::Ceil( 0.4_fx ) == 1_fx );

            TEST_ASSERT( Fixed::Ceil( 1.5_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Ceil( -1.5_fx ) == -1_fx );
            TEST_ASSERT( Fixed::Ceil( 3_fx ) == 3_fx );
            TEST_ASSERT( Fixed::Ceil( -3_fx ) == -3_fx );
        }

        void TestRound()
        {
            static_assert( Fixed::Round( 1.6_fx ) == 2_fx );

            TEST_ASSERT( Fixed::Round( 1.6_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Round( 1.4_fx ) == 1_fx );
            TEST_ASSERT( Fixed::Round( -1.6_fx ) == -2_fx );
            TEST_ASSERT( Fixed::Round( -1.4_fx ) == -1_fx );

            TEST_ASSERT( Fixed::Round( 1.5_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Round( -1.5_fx ) == -1_fx );
        }

        void TestAbs()
        {
            static_assert( Fixed::Abs( -1_fx ) == 1_fx );

            TEST_ASSERT( Fixed::Abs( 1.5_fx ) == 1.5_fx )
            TEST_ASSERT( Fixed::Abs( -1.5_fx ) == 1.5_fx )
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
            static_assert( Fixed::PowI( 4_fx, 2 ) == 16_fx );

            TEST_ASSERT( Fixed::PowI( 1.5_fx, 0 ) == 1_fx );
            TEST_ASSERT( Fixed::PowI( 1.5_fx, 2 ) == 2.25_fx );
            TEST_ASSERT( Fixed::PowI( 2_fx, 2 ) == 4_fx );
            TEST_ASSERT( Fixed::PowI( -2_fx, 3 ) == -8_fx );
            TEST_ASSERT( Fixed::PowI( 3.5_fx, 3 ) == 42.875_fx );
        }

        void TestSin()
        {
            static_assert( Fixed::Sin( 0_fx ) == 0_fx );

            TEST_ASSERT( Fixed::IsFuzzyZero( FX_PI - FIXED( 3.141592654 ) ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( FX_PI ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( FX_HALF_PI ) - 1 ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Sin( -FX_HALF_PI ) - ( -1 ) ) );
            TEST_ASSERT( Fixed::Sin( FX_PI + 1_fx ) == Fixed::Sin( -FX_PI + 1_fx ) );
            TEST_ASSERT( Fixed::Sin( -FX_PI - 1_fx ) == Fixed::Sin( FX_PI - 1_fx ) );

            FixedFunction  fxSin     = &Fixed::Sin;
            DoubleFunction doubleSin = &std::sin;
            double         error     = MaxErrorFixedVsDouble( fxSin,
                                                              doubleSin,
                                                              ( -FX_TWO_PI ).ToDouble(),
                                                              ( FX_TWO_PI ).ToDouble(),
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
            static_assert( Fixed::Cos( 0_fx ) == 1_fx );

            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( FX_PI ) - ( -1 ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( FX_HALF_PI ) ) );
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed::Cos( -FX_HALF_PI ) ) );

            TEST_ASSERT( Fixed::Cos( FX_PI + 1_fx ) == Fixed::Cos( -FX_PI + 1_fx ) );
            TEST_ASSERT( Fixed::Cos( -FX_PI - 1_fx ) == Fixed::Cos( FX_PI - 1_fx ) );

            FixedFunction  fxCos     = &Fixed::Cos;
            DoubleFunction doubleCos = &std::cos;
            double         error     = MaxErrorFixedVsDouble( fxCos,
                                                              doubleCos,
                                                              ( -FX_TWO_PI ).ToDouble(),
                                                              ( FX_TWO_PI ).ToDouble(),
                                                              0.0001 );
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
            static_assert( Fixed::Sqrt( 4_fx ) == 2_fx );

            TEST_ASSERT( Fixed::Sqrt( 0_fx ) == 0_fx )
            TEST_ASSERT( Fixed::Sqrt( 1.75_fx * 1.75_fx ) == 1.75_fx )

            FixedFunction  fxSqrt     = &Fixed::Sqrt;
            DoubleFunction doubleSqrt = &std::sqrt;
            double         error      = MaxErrorFixedVsDouble( fxSqrt, doubleSqrt, 0, 1., 0.0001 );
            TEST_ASSERT( error < 0.0005 ) // [0,1]
            error = MaxErrorFixedVsDouble( fxSqrt, doubleSqrt, 1., Fixed::sMaxInteger, 0.1 );
            TEST_ASSERT( error < 0.0005 )// [1,max]
        }
    };
}