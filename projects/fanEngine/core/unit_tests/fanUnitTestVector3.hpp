#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanVector3.hpp"
#include "core/math/fanMathUtils.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestVector3 : public UnitTest<UnitTestVector3>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestVector3::TestConstructors,     "constructors" },
                     { &UnitTestVector3::TestMagnitude,        "magnitude" },
                     { &UnitTestVector3::TestDistance,         "distance" },
                     { &UnitTestVector3::TestNormalize,        "normalize" },
                     { &UnitTestVector3::TestComparison,       "comparison" },
                     { &UnitTestVector3::TestMultiplication,   "multiplication" },
                     { &UnitTestVector3::TestDivision,         "division" },
                     { &UnitTestVector3::TestAddSubstract,     "addition subtraction" },
                     { &UnitTestVector3::TestDot,              "dot" },
                     { &UnitTestVector3::TestCross,            "cross" },
                     { &UnitTestVector3::TestOrthoNormalize,   "ortho normalize" },
                     { &UnitTestVector3::TestAngle,            "angle" },
                     { &UnitTestVector3::TestSignedAngle,      "signed angle" },
                     { &UnitTestVector3::TestOrthonormalBasis, "orthonormal basis" },
                     { &UnitTestVector3::TestArrayOperator,    "array operator" },
            };
        }

        void Create() override
        {
        }

        void Destroy() override
        {
        }

        void TestConstructors()
        {
            Vector3 vec3;
            TEST_ASSERT( vec3.x == 0 && vec3.y == 0 && vec3.z == 0 )

            vec3 = { 1, 2, 3 };
            TEST_ASSERT( vec3.x == FIXED( 1 ) )
            TEST_ASSERT( vec3.y == FIXED( 2 ) )
            TEST_ASSERT( vec3.z == FIXED( 3 ) )

            Vector3 cpy( vec3 );
            TEST_ASSERT( vec3.x == cpy.x )
            TEST_ASSERT( vec3.y == cpy.y )
            TEST_ASSERT( vec3.z == cpy.z )
        }

        void TestMagnitude()
        {
            // sqrMagnitude
            TEST_ASSERT( Vector3( -4, 0, 0 ).SqrMagnitude() == 16 )
            TEST_ASSERT( Vector3( 1, -1, 0 ).SqrMagnitude() == 2 )
            TEST_ASSERT( Vector3( -1, 2, -3 ).SqrMagnitude() == 14 )

            // magnitude
            TEST_ASSERT( Vector3( -4, 0, 0 ).Magnitude() == 4 )
            TEST_ASSERT( Vector3( 1, -1, 0 ).Magnitude() == Fixed::Sqrt( 2 ) )
            TEST_ASSERT( Vector3( -1, 2, -3 ).Magnitude() == Fixed::Sqrt( 14 ) )
        }

        void TestDistance()
        {
            // sqrDistance
            TEST_ASSERT( Vector3::SqrDistance( { -4, 0, 0 }, { 1, 0, 0 } ) == 25 )
            TEST_ASSERT( Vector3::SqrDistance( { 1, 2, 0 }, { 3, 4, 0 } ) == 8 )
            TEST_ASSERT( Vector3::SqrDistance( { 1, 2, 3 }, { 4, 5, 6 } ) == 27 )

            // distance
            TEST_ASSERT( Vector3::Distance( { -4, 0, 0 }, { 1, 0, 0 } ) == 5 )
            TEST_ASSERT( Vector3::Distance( { 1, 2, 0 }, { 3, 4, 0 } ) == Fixed::Sqrt( 8 ) )
            TEST_ASSERT( Vector3::Distance( { 1, 2, 3 }, { 4, 5, 6 } ) == Fixed::Sqrt( 27 ) )
        }

        void TestNormalize()
        {
            Vector3 vec3( 2, 0, 0 );
            vec3.Normalize();
            TEST_ASSERT( vec3.Magnitude() == 1 )
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed( 1 ) - Vector3( 1, 2, 3 ).Normalized().Magnitude() ) )

            vec3 = { FIXED( 55.4 ), FIXED( 33.6 ), FIXED( 44.8 ) };
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed( 1 ) - vec3.Normalized().Magnitude() ) )

            vec3 = { FIXED( -12.867 ), FIXED( 4.6 ), FIXED( -25.8 ) };
            TEST_ASSERT( Fixed::IsFuzzyZero( Fixed( 1 ) - vec3.Normalized().Magnitude() ) )

            // is normalized
            TEST_ASSERT( Vector3::sUp.IsNormalized() )
            TEST_ASSERT( !Vector3::sZero.IsNormalized() )
            TEST_ASSERT( !( 2 * Vector3::sUp ).IsNormalized() )
        }

        void TestComparison()
        {
            TEST_ASSERT( Vector3( 2, -4, 6 ) == Vector3( 2, -4, 6 ) )
            Vector3 vec3 = { FIXED( 55.4 ), FIXED( 33.6 ), FIXED( 44.8 ) };
            TEST_ASSERT( vec3 == vec3 )
            TEST_ASSERT( vec3 != Vector3( FIXED( 55.3 ), FIXED( 33.6 ), FIXED( 44.8 ) ) )
            TEST_ASSERT( vec3 != Vector3( FIXED( 55.4 ), FIXED( -33.6 ), FIXED( 44.8 ) ) )
        }

        void TestMultiplication()
        {
            Vector3 vec3( 1, -2, 3 );
            vec3 *= 2;
            TEST_ASSERT( vec3 == Vector3( 2, -4, 6 ) )

            vec3 = Vector3( 1, -2, 3 );
            vec3 *= -2;
            TEST_ASSERT( vec3 == Vector3( -2, 4, -6 ) )

            vec3 = { FIXED( 54.4 ), FIXED( -32.6 ), FIXED( 44.8 ) };
            vec3 *= FIXED( -0.5 );
            TEST_ASSERT( vec3 == Vector3( FIXED( -27.2 ), FIXED( 16.3 ), FIXED( -22.4 ) ) )

            Fixed value = FIXED( 78.6 );
            TEST_ASSERT( value * vec3 == ( vec3 *= value ) )
            TEST_ASSERT( value * vec3 == vec3 * value )

            TEST_ASSERT( Vector3::Mult( Vector3( 2, -3, 4 ), Vector3( 5, 6, 7 ) ) == Vector3( 10, -18, 28 ) )
        }

        void TestDivision()
        {
            Vector3 vec3( 2, -4, 6 );
            vec3 /= 2;
            TEST_ASSERT( vec3 == Vector3( 1, -2, 3 ) )

            vec3 = Vector3( 2, -4, 6 );
            vec3 /= -2;
            TEST_ASSERT( vec3 == Vector3( -1, 2, -3 ) )

            vec3 = { FIXED( 54.4 ), FIXED( -32.6 ), FIXED( 44.8 ) };
            vec3 /= -2;
            TEST_ASSERT( vec3 == Vector3( FIXED( -27.2 ), FIXED( 16.3 ), FIXED( -22.4 ) ) )

            TEST_ASSERT( vec3 / 2 == ( vec3 /= 2 ) )
        }

        void TestAddSubstract()
        {
            TEST_ASSERT( Vector3( 1, 2, 3 ) + Vector3( 4, -5, 6 ) == Vector3( 5, -3, 9 ) )
            TEST_ASSERT( Vector3( 1, 2, 3 ) + Vector3() == Vector3( 1, 2, 3 ) )

            TEST_ASSERT( Vector3( 1, 2, 3 ) - Vector3( 4, -5, 6 ) == Vector3( -3, 7, -3 ) )
            TEST_ASSERT( Vector3( 1, 2, 3 ) - Vector3() == Vector3( 1, 2, 3 ) )

            Vector3 vec1( FIXED( 54.4 ), FIXED( -32.3 ), FIXED( 44.8 ) );
            Vector3 vec2( FIXED( -16.1 ), FIXED( 26.6 ), FIXED( 1.15 ) );
            TEST_ASSERT( vec1 + vec2 == vec2 + vec1 )
            TEST_ASSERT( vec1 - vec2 == -( vec2 - vec1 ) )
        }

        void TestDot()
        {
            Vector3 vec1( 1, 2, 3 );
            Vector3 vec2( 4, 5, 6 );
            TEST_ASSERT( Vector3::Dot( vec1, vec2 ) == 32 )
            TEST_ASSERT( Vector3::Dot( vec1, vec2 ) == Vector3::Dot( vec2, vec1 ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Vector3::Dot( vec1, vec1 ) - vec1.Magnitude() * vec1.Magnitude() ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Vector3::Dot( vec2, vec2 ) - vec2.Magnitude() * vec2.Magnitude() ) )
        }

        void TestCross()
        {
            // axis
            TEST_ASSERT( Vector3::Cross( Vector3::sUp, Vector3::sRight ) == Vector3::sForward );
            TEST_ASSERT( Vector3::Cross( Vector3::sForward, Vector3::sUp ) == Vector3::sRight );
            TEST_ASSERT( Vector3::Cross( Vector3::sRight, Vector3::sForward ) == Vector3::sUp );

            // identities
            Vector3 v1( 1, 2, 3 );
            Vector3 v2( 4, 5, 6 );
            Vector3 v3( 7, 8, 9 );
            Fixed   s = 42;
            TEST_ASSERT( Vector3::Cross( v1, v2 ) == -Vector3::Cross( v2, v1 ) );
            TEST_ASSERT( s * Vector3::Cross( v1, v2 ) == Vector3::Cross( s * v1, v2 ) )
            TEST_ASSERT( s * Vector3::Cross( v1, v2 ) == Vector3::Cross( v1, s * v2 ) )
            TEST_ASSERT( Vector3::Cross( v1, v2 + v3 ) == Vector3::Cross( v1, v2 ) + Vector3::Cross( v1, v3 ) )
        }

        void TestOrthoNormalize()
        {
            Vector3 v1 = Vector3::sUp;
            Vector3 v2 = Vector3( 0, 1, 1 );
            Vector3::OrthoNormalize( v1, v2 );
            TEST_ASSERT( v1 == Vector3::sUp )
            TEST_ASSERT( v2 == Vector3::sForward )
            TEST_ASSERT( Vector3::Dot( v1, v2 ) == 0 )
            TEST_ASSERT( v1.Magnitude() == 1 )
            TEST_ASSERT( v2.Magnitude() == 1 )

            v1 = Vector3( -5, 4, 7 ).Normalized();
            v2 = Vector3( 1, -6, -2 );
            Vector3::OrthoNormalize( v1, v2 );
            TEST_ASSERT( Fixed::IsFuzzyZero( v1.Magnitude() - 1 ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( v2.Magnitude() - 1 ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Vector3::Dot( v1, v2 ) ) )
            TEST_ASSERT( Fixed::IsFuzzyZero( Vector3::Dot( v1, v1 ) - v1.SqrMagnitude() ) )
        }

        void TestAngle()
        {
            const Fixed angularTolerance = FIXED( 0.005 );

            Fixed angle = Vector3::Angle( Vector3::sForward, Vector3::sForward );
            TEST_ASSERT( Fixed::Abs( angle ) < angularTolerance )

            angle = Vector3::Angle( Vector3::sForward, Vector3::sForward );
            TEST_ASSERT( Fixed::Abs( angle ) < angularTolerance )

            angle = Vector3::Angle( Vector3::sForward, Vector3::sUp );
            TEST_ASSERT( Fixed::Abs( angle ) - 90 < angularTolerance )

            angle = Vector3::Angle( Vector3::sForward, Vector3::sUp );
            TEST_ASSERT( Fixed::Abs( angle ) - 90 < angularTolerance )

            angle = Vector3::Angle( Vector3::sForward, Vector3::sBack );
            TEST_ASSERT( Fixed::Abs( angle ) - 180 < angularTolerance )
        }

        void TestSignedAngle()
        {
            const Fixed angularTolerance = FIXED( 0.005 );

            Fixed angle = Vector3::SignedAngle( Vector3::sForward, Vector3::sForward, Vector3::sForward );
            TEST_ASSERT( Fixed::Abs( angle - 0 ) < angularTolerance )

            angle = Vector3::SignedAngle( Vector3::sForward, Vector3::sForward, Vector3::sUp );
            TEST_ASSERT( Fixed::Abs( angle - 0 ) < angularTolerance )

            angle = Vector3::SignedAngle( Vector3::sForward, Vector3::sUp, Vector3::sRight );
            TEST_ASSERT( Fixed::Abs( angle - 90 ) < angularTolerance )

            angle = Vector3::SignedAngle( Vector3::sForward, Vector3::sUp, -Vector3::sRight );
            TEST_ASSERT( Fixed::Abs( angle - ( -90 ) ) < angularTolerance )

            angle = Vector3::SignedAngle( Vector3::sForward, Vector3::sBack, -Vector3::sUp );
            TEST_ASSERT( Fixed::Abs( angle - ( 180 ) ) < angularTolerance )
        }

        void TestOrthonormalBasis()
        {
            Vector3 v1, v2, v3;

            v1 = Vector3::sLeft;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )

            v1 = Vector3::sRight;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )

            v1 = Vector3::sUp;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )

            v1 = Vector3::sDown;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )

            v1 = Vector3::sForward;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )

            v1 = Vector3::sBack;
            Vector3::MakeOrthonormalBasis( v1, v2, v3 );
            TEST_ASSERT( v2.IsNormalized() && v3.IsNormalized() )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v1, v2 ) - v3 ) )
            TEST_ASSERT( Vector3::IsFuzzyZero( Vector3::Cross( v2, v3 ) - v1 ) )
        }

        void TestArrayOperator()
        {
            Vector3 v( 1, 2, 3 );
            TEST_ASSERT( v[0] == 1 );
            TEST_ASSERT( v[1] == 2 );
            TEST_ASSERT( v[2] == 3 );
        }
    };
}












