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
                     { &UnitTestFixedPoint::TestFloor,          "Floor" },
                     { &UnitTestFixedPoint::TestCeil,           "Ceil" },
                     { &UnitTestFixedPoint::TestRound,          "Round" },
                     { &UnitTestFixedPoint::TestAbs,            "Abs" },
                     { &UnitTestFixedPoint::TestPowI,           "PowI" },
                     { &UnitTestFixedPoint::TestSin,            "Sin" },
            };
        }

        void Create() override
        {
        }

        void Destroy() override
        {
        }

        void TestIntegers()
        {
            Fixed f1( 4 );
            int   value  = f1.ToInt();
            TEST_ASSERT( value == 4 );

            Fixed f2( -4 );
            int   value2 = f2.ToInt();
            TEST_ASSERT( value2 == -4 );

            TEST_ASSERT( Fixed( Fixed::sMaxInteger ).ToInt() == Fixed::sMaxInteger );
            TEST_ASSERT( Fixed( Fixed::sMaxInteger + 1 ).ToInt() != Fixed::sMaxInteger + 1 );
            TEST_ASSERT( Fixed( Fixed::sMinInteger ).ToInt() == Fixed::sMinInteger );
            TEST_ASSERT( Fixed( Fixed::sMinInteger - 1 ).ToInt() != Fixed::sMinInteger - 1 );
        }

        void TestFloats()
        {
            TEST_ASSERT( Fixed( 5.0625f ).ToFloat() == 5.0625f );
            TEST_ASSERT( Fixed( -5.0625f ).ToFloat() == -5.0625f );
            TEST_ASSERT( Fixed( float( Fixed::sMaxInteger ) ).ToFloat() == Fixed::sMaxInteger );
            TEST_ASSERT( Fixed( float( Fixed::sMax ) + 1.f ).ToFloat() != Fixed::sMax + 1.f );
            TEST_ASSERT( Fixed( float( Fixed::sMin ) ).ToFloat() == Fixed::sMin );
            TEST_ASSERT( Fixed( float( Fixed::sMin ) - 1.f ).ToFloat() != Fixed::sMin - 1.f );
        }

        void TestDoubles()
        {
            TEST_ASSERT( Fixed( Fixed::sMinFractional ).ToDouble() == Fixed::sMinFractional );
            TEST_ASSERT( Fixed( Fixed::sMinFractional / 2. ).ToDouble() != Fixed::sMinFractional / 2. );
            TEST_ASSERT( Fixed( Fixed::sMaxFractional ).ToDouble() == Fixed::sMaxFractional );
            TEST_ASSERT( Fixed( Fixed::sMaxFractional + Fixed::sMinFractional ).ToDouble() == 1. );
            TEST_ASSERT( Fixed( Fixed::sMax ).ToDouble() == Fixed::sMax );
            TEST_ASSERT( Fixed( Fixed::sMin ).ToDouble() == Fixed::sMin );
            TEST_ASSERT( Fixed( Fixed::sMax + Fixed::sMinFractional ).ToDouble() != Fixed::sMax + Fixed::sMinFractional );
            TEST_ASSERT( Fixed( Fixed::sMin - Fixed::sMinFractional ).ToDouble() != Fixed::sMin - Fixed::sMinFractional );
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
            static_assert(  1_fx < 2_fx);

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
            static_assert(  1_fx + 2_fx == 3_fx );

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
            static_assert(  1_fx - 3_fx == -2_fx );

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
            static_assert(  2_fx * 3_fx == 6_fx );

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
            static_assert(  6_fx / 2_fx == 3_fx );

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
            static_assert(  3_fx % 2_fx == 1_fx );

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

        void TestFloor()
        {
            static_assert(  Fixed::Floor( 1.6_fx) == 1_fx );

            TEST_ASSERT( Fixed::Floor( 1.5_fx ) == 1_fx );
            TEST_ASSERT( Fixed::Floor( -1.5_fx ) == -2_fx );
            TEST_ASSERT( Fixed::Floor( 3_fx ) == 3_fx );
            TEST_ASSERT( Fixed::Floor( -3_fx ) == -3_fx );
        }

        void TestCeil()
        {
            static_assert(  Fixed::Ceil( 0.4_fx) == 1_fx );

            TEST_ASSERT( Fixed::Ceil( 1.5_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Ceil( -1.5_fx ) == -1_fx );
            TEST_ASSERT( Fixed::Ceil( 3_fx ) == 3_fx );
            TEST_ASSERT( Fixed::Ceil( -3_fx ) == -3_fx );
        }

        void TestRound()
        {
            static_assert(  Fixed::Round( 1.6_fx) == 2_fx );

            TEST_ASSERT( Fixed::Round( 1.6_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Round( 1.4_fx ) == 1_fx );
            TEST_ASSERT( Fixed::Round( -1.6_fx ) == -2_fx );
            TEST_ASSERT( Fixed::Round( -1.4_fx ) == -1_fx );

            TEST_ASSERT( Fixed::Round( 1.5_fx ) == 2_fx );
            TEST_ASSERT( Fixed::Round( -1.5_fx ) == -1_fx );
        }

        void TestAbs()
        {
            static_assert(  Fixed::Abs( -1_fx) == 1_fx );

            TEST_ASSERT( Fixed::Abs( 1.5_fx ) == 1.5_fx );
            TEST_ASSERT( Fixed::Abs( -1.5_fx ) == 1.5_fx );
        }

        void TestPowI()
        {
            static_assert(  Fixed::PowI( 4_fx, 2) == 16_fx );

            TEST_ASSERT( Fixed::PowI( 1.5_fx, 2 ) == 2.25_fx );
            TEST_ASSERT( Fixed::PowI( 2_fx, 2 ) == 4_fx );
            TEST_ASSERT( Fixed::PowI( -2_fx, 3 ) == -8_fx );
            TEST_ASSERT( Fixed::PowI( 3.5_fx, 3 ) == 42.875_fx );
        }

        void TestSin()
        {
            static_assert(  Fixed::Sin( 0_fx ) == 0_fx );

            // we want at least two digits of precision for now
            TEST_ASSERT( Fixed::Floor( FX_PI * 10000_fx ) == 31415_fx )
            TEST_ASSERT( Fixed::Sin( 0_fx ) == 0_fx );
            TEST_ASSERT( Fixed::Floor( Fixed::Sin( FX_PI ) * 100_fx ) == 0_fx );
            TEST_ASSERT( Fixed::Floor( Fixed::Sin( FX_HALF_PI ) ) * 100_fx == 100_fx );
            TEST_ASSERT( Fixed::Floor( Fixed::Sin( -FX_HALF_PI ) ) * 100_fx == -100_fx );

            TEST_ASSERT( Fixed::Sin( 3_fx*FX_PI ) == Fixed::Sin(FX_PI));
            TEST_ASSERT( Fixed::Sin( -5_fx*FX_PI ) == Fixed::Sin(-FX_PI));
        }
    };
}