#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    //======================================================================================================================
    //======================================================================================================================
    class UnitTestQuaternion : public UnitTest<UnitTestQuaternion>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestQuaternion::TestConstructors,   "constructors" },
                     { &UnitTestQuaternion::TestMagnitude,      "magnitude" },
                     { &UnitTestQuaternion::TestComparison,     "comparison" },
                     { &UnitTestQuaternion::TestAddition,       "add" },
                     { &UnitTestQuaternion::TestSubstraction,   "substract" },
                     { &UnitTestQuaternion::TestMultiply,       "multiply" },
                     { &UnitTestQuaternion::TestDivide,         "divide" },
                     { &UnitTestQuaternion::TestConjugate,      "conjugate" },
                     { &UnitTestQuaternion::TestEuler,          "euler" },
                     { &UnitTestQuaternion::TestAngle,          "angle" },
                     { &UnitTestQuaternion::TestAxis,           "axis" },
                     { &UnitTestQuaternion::TestEulerRotations, "euler rotation" },
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
            Quaternion q;
            TEST_ASSERT( q.n == 1 )
            TEST_ASSERT( q.v == Vector3::sZero )

            Quaternion q1( 1, 2, 3, 4 );
            TEST_ASSERT( q1.n == 1 )
            TEST_ASSERT( q1.v == Vector3( 2, 3, 4 ) )

            TEST_ASSERT( Quaternion( 1, 2, 3, 4 ) == Quaternion( 1, { 2, 3, 4 } ) )

            TEST_ASSERT( Quaternion() * Vector3(10,20,30) == Vector3(10,20,30) )
        }

        void TestMagnitude()
        {
            Quaternion q;
            TEST_ASSERT( q.Magnitude() == 1 );

            q = Quaternion( 1, 1, 1, 1 );
            TEST_ASSERT( q.Magnitude() == 2 );

            q = Quaternion( -1, 1, -1, 1 );
            TEST_ASSERT( q.Magnitude() == 2 );

            q = Quaternion( 1, 2, 3, 4 );
            TEST_ASSERT( q.Magnitude() == Fixed::Sqrt( 30 ) );
        }

        void TestComparison()
        {
            Quaternion q1(0,0,0,0);
            TEST_ASSERT( q1 == q1 );
            TEST_ASSERT( Quaternion( 1, 0, 0, 0 ) != q1 );
            TEST_ASSERT( Quaternion( 0, 1, 0, 0 ) != q1 );
            TEST_ASSERT( Quaternion( 0, 0, 1, 0 ) != q1 );
            TEST_ASSERT( Quaternion( 0, 0, 0, 1 ) != q1 );
        }

        void TestAddition()
        {
            Quaternion q1( 1, 4, 3, 2 );
            Quaternion q2( 7, 6, 5, 8 );
            Quaternion q3( 12, 10, 11, 9 );
            TEST_ASSERT( q1 + q2 == q2 + q1 )
            TEST_ASSERT( ( q1 + q2 ) + q3 == q1 + ( q2 + q3 ) )
            TEST_ASSERT( q1 + Quaternion(0,0,0,0) == q1 )
            TEST_ASSERT( q1 + q3 == Quaternion( 13, 14, 14, 11 ) )

            TEST_ASSERT( ( q1 += Quaternion( 1, 1, 1, 1 ) ) == Quaternion( 2, 5, 4, 3 ) )
        }

        void TestSubstraction()
        {
            Quaternion q1( 1, 4, 3, 2 );
            Quaternion q2( 7, 6, 5, 8 );
            TEST_ASSERT( q2 - q1 == Quaternion( 6, 2, 2, 6 ) )
            TEST_ASSERT( q1 - q2 == q1 + ( -q2 ) )
            TEST_ASSERT( -q1 == Quaternion(0,0,0,0) - q1 )
            TEST_ASSERT( ( q2 -= q1 ) == Quaternion( 6, 2, 2, 6 ) )
        }

        void TestMultiply()
        {
            // scalar multiplication
            Quaternion q( 6, 4, 3, 2 );
            TEST_ASSERT( q * 0 == Quaternion(0,0,0,0) )
            TEST_ASSERT( q * 1 == q )
            TEST_ASSERT( q * 2 == Quaternion( 12, 8, 6, 4 ) )
            TEST_ASSERT( q * 2 == 2 * q )

            // quaternion multiplication
            Quaternion p( 13, 14, 14, 11 );
            Quaternion h( 7, 6, 5, 8 );
            TEST_ASSERT( q * ( p * h ) == ( q * p ) * h );
            TEST_ASSERT( q * p != p * q );
        }

        void TestDivide()
        {
            // scalar divide
            Quaternion q( 6, 4, 10, 16 );
            TEST_ASSERT( q / 1 == q )
            TEST_ASSERT( q / 2 == Quaternion( 3, 2, 5, 8 ) )
            TEST_ASSERT( q / 2 == FIXED( 0.5 ) * q )
        }

        void TestConjugate()
        {
            Quaternion q( 6, 4, 3, 2 );
            Quaternion p( 1, 12, 17, 9 );
            TEST_ASSERT( ( q * p ).Conjugate() == p.Conjugate() * q.Conjugate() )
        }

        void TestEuler()
        {
            const Fixed angleMinDelta = FIXED( .01 );

            Vector3    eulerIn   = Vector3( 10, 20, 30 );
            Quaternion eulerQuat = Quaternion::FromEuler( eulerIn );
            Vector3    eulerOut  = eulerQuat.Euler();
            TEST_ASSERT( Fixed::Abs( eulerIn.x - eulerOut.x ) < angleMinDelta )
            TEST_ASSERT( Fixed::Abs( eulerIn.y - eulerOut.y ) < angleMinDelta )
            TEST_ASSERT( Fixed::Abs( eulerIn.z - eulerOut.z ) < angleMinDelta )

            eulerIn   = Vector3( -5, 17, 225 );
            eulerQuat = Quaternion::FromEuler( eulerIn );
            eulerOut  = eulerQuat.Euler();
            TEST_ASSERT( ( Fixed::Abs( eulerIn.x - eulerOut.x ) + 360 ) % 360 < angleMinDelta )
            TEST_ASSERT( ( Fixed::Abs( eulerIn.y - eulerOut.y ) + 360 ) % 360 < angleMinDelta )
            TEST_ASSERT( ( Fixed::Abs( eulerIn.z - eulerOut.z ) + 360 ) % 360 < angleMinDelta )
        }

        void TestAngle()
        {
            const Fixed angleMinDelta = FIXED( .02 );
            Fixed       angle         = Fixed::Degrees( Quaternion::FromEuler( 10, 0, 0 ).Angle() );
            TEST_ASSERT( Fixed::Abs( angle - 10 ) < angleMinDelta )

            angle = Fixed::Degrees( Quaternion::FromEuler( 10, 0, 0 ).Angle() );
            TEST_ASSERT( Fixed::Abs( angle - 10 ) < angleMinDelta )

            angle = Fixed::Degrees( Quaternion::FromEuler( 0, 5, 0 ).Angle() );
            TEST_ASSERT( Fixed::Abs( angle - 5 ) < angleMinDelta )

            angle = Fixed::Degrees( Quaternion::FromEuler( 0, 0, 44 ).Angle() );
            TEST_ASSERT( Fixed::Abs( angle - 44 ) < angleMinDelta )

            angle = Fixed::Degrees( Quaternion::FromEuler( 0, 0, 0 ).Angle() );
            TEST_ASSERT( Fixed::IsFuzzyZero( angle ) )
        }

        void TestAxis()
        {
            Vector3 axis = Quaternion::FromEuler( 90, 0, 0 ).Axis();
            TEST_ASSERT( Fixed::IsFuzzyZero( ( axis - Vector3::sLeft ).SqrMagnitude() ) )

            axis = Quaternion::FromEuler( 0, 45, 0 ).Axis();
            TEST_ASSERT( Fixed::IsFuzzyZero( ( axis - Vector3::sUp ).SqrMagnitude() ) )

            axis = Quaternion::FromEuler( 0, 0, -30 ).Axis();
            TEST_ASSERT( Fixed::IsFuzzyZero( ( axis - Vector3::sBack ).SqrMagnitude() ) )
        }

        void TestEulerRotations()
        {
            Quaternion rotateX = Quaternion::FromEuler( 90, 0, 0 );
            TEST_ASSERT( rotateX * Vector3::sUp == Vector3::sForward )
            TEST_ASSERT( rotateX * Vector3::sForward == Vector3::sDown )
            TEST_ASSERT( rotateX * Vector3::sDown == Vector3::sBack )
            TEST_ASSERT( rotateX * Vector3::sBack == Vector3::sUp )

            Quaternion rotateY = Quaternion::FromEuler( 0, 90, 0 );
            TEST_ASSERT( rotateY * Vector3::sRight == Vector3::sForward )
            TEST_ASSERT( rotateY * Vector3::sForward == Vector3::sLeft )
            TEST_ASSERT( rotateY * Vector3::sLeft == Vector3::sBack )
            TEST_ASSERT( rotateY * Vector3::sBack == Vector3::sRight )

            Quaternion rotateZ = Quaternion::FromEuler( 0, 0, -90 );
            TEST_ASSERT( rotateZ * Vector3::sUp == Vector3::sLeft )
            TEST_ASSERT( rotateZ * Vector3::sLeft == Vector3::sDown )
            TEST_ASSERT( rotateZ * Vector3::sDown == Vector3::sRight )
            TEST_ASSERT( rotateZ * Vector3::sRight == Vector3::sUp )

            Vector3 rotated = Quaternion::FromEuler( 0, 45, 0 ) * Vector3::sForward;
            TEST_ASSERT( rotated.y == 0 )
            TEST_ASSERT( rotated.x == Fixed::Sqrt( 2 ) / 2 )
            TEST_ASSERT( Fixed::IsFuzzyZero( rotated.z - rotated.x ) )

            Vector3 unRotated = ( Quaternion::FromEuler( 0, -45, 0 ) * Quaternion::FromEuler( 0, 45, 0 ) ) * Vector3::sForward;
            TEST_ASSERT( unRotated == Vector3::sForward )
            unRotated = Quaternion::FromEuler( 0, -45, 0 ) * ( Quaternion::FromEuler( 0, 45, 0 ) * Vector3::sForward );
            TEST_ASSERT( Fixed::IsFuzzyZero( ( unRotated - Vector3::sForward ).Magnitude() ) )
        }
    };
}