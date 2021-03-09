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
                     { &UnitTestFixedPoint::TestAddition,       "Addition" },
                     { &UnitTestFixedPoint::TestSubtraction,    "Subtraction" },
                     { &UnitTestFixedPoint::TestMultiplication, "Addition" },
                     { &UnitTestFixedPoint::TestDivision,       "Division" },
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
            Fixed f1( 5.00390625f );
            float value  = f1.ToFloat();
            TEST_ASSERT( value == 5.00390625f );

            Fixed f2( -5.00390625f );
            float value2 = f2.ToFloat();
            TEST_ASSERT( value2 == -5.00390625f );

            TEST_ASSERT( Fixed( 32767.f ).ToFloat() == 32767.f );
            TEST_ASSERT( Fixed( -32768.f ).ToFloat() == -32768.f );
            TEST_ASSERT( Fixed( 32767.f ).ToFloat() != 32768.f );
            TEST_ASSERT( Fixed( -32768.f ).ToFloat() != -32769.f );
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
            TEST_ASSERT( ( 123_fx ).ToFloat() == 123.f );
            TEST_ASSERT( ( .75_fx ).ToFloat() == .75f );
            TEST_ASSERT( ( 0000.0625_fx ).ToFloat() == .0625f );
            TEST_ASSERT( ( 0.625_fx ).ToFloat() == 0.625f );
            TEST_ASSERT( ( 0.0001220703125_fx ).ToFloat() == 0.0001220703125 );
            TEST_ASSERT( Fixed( "-32768.5" ).ToDouble() == -32768.5 );
            TEST_ASSERT( Fixed( "-32769." ).ToDouble() != -32769. );
            TEST_ASSERT( Fixed( 32767.99993896484375_fx ).ToDouble() == 32767.99993896484375 );
            TEST_ASSERT( Fixed( "-32767.00006103515625" ).ToDouble() == -32767.00006103515625 );
            TEST_ASSERT( Fixed( Fixed::sMaxFractional ).ToDouble() == 0.99993896484375 );
        }

        void TestAddition()
        {
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
            // (-)
            TEST_ASSERT( 4.5_fx - 1.25_fx == 3.25_fx );
            TEST_ASSERT( 1.25_fx - 4.5_fx == -3.25_fx );
            TEST_ASSERT( 1.25_fx - 0_fx == 1.25_fx );
            // (-=)
            TEST_ASSERT( ( 4.5_fx -= 1.25_fx ) == 3.25_fx );
            // --
            Fixed g = 1.5_fx;
            TEST_ASSERT( --g == 0.5_fx );
            TEST_ASSERT( g-- == 0.5_fx );
            TEST_ASSERT( g == -0.5_fx );
        }

        void TestMultiplication()
        {
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
            // (/)
            TEST_ASSERT( 6_fx / 2_fx == 3_fx );
            TEST_ASSERT( -6_fx / 2_fx == -3_fx );
            TEST_ASSERT( 1_fx / 2_fx == .5_fx );
            TEST_ASSERT( 1_fx / -2_fx == -.5_fx );

            // (/=)
            TEST_ASSERT( ( 8_fx /= 2_fx ) == 4._fx );
            TEST_ASSERT( ( 4.5_fx /= -2_fx ) == -2.25_fx );
        }
    };
}