#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanMatrix3.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestMatrix3 : public UnitTest<UnitTestMatrix3>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestMatrix3::TestConstructors,         "constructors" },
                     { &UnitTestMatrix3::TestComparison,           "comparison" },
                     { &UnitTestMatrix3::TestAddition,             "addition" },
                     { &UnitTestMatrix3::TestSubtraction,          "subtraction" },
                     { &UnitTestMatrix3::TestMultiplication,       "multiplication" },
                     { &UnitTestMatrix3::TestDivision,             "division" },
                     { &UnitTestMatrix3::TestDeterminant,          "determinant" },
                     { &UnitTestMatrix3::TestTranspose,            "transpose" },
                     { &UnitTestMatrix3::TestInverse,              "inverse" },
                     { &UnitTestMatrix3::TestToQuaternion,         "to quaternion" },
                     { &UnitTestMatrix3::TestVectorMultiplication, "vector multiplication" },
                     { &UnitTestMatrix3::TestSkewSymmetric,        "skew symmetric" },
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
            Matrix3 mat;
            TEST_ASSERT( mat.e11 == 0 )
            TEST_ASSERT( mat.e12 == 0 )
            TEST_ASSERT( mat.e13 == 0 )
            TEST_ASSERT( mat.e21 == 0 )
            TEST_ASSERT( mat.e22 == 0 )
            TEST_ASSERT( mat.e23 == 0 )
            TEST_ASSERT( mat.e31 == 0 )
            TEST_ASSERT( mat.e32 == 0 )
            TEST_ASSERT( mat.e33 == 0 )

            Matrix3 mat2( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat2.e11 == 1 )
            TEST_ASSERT( mat2.e12 == 2 )
            TEST_ASSERT( mat2.e13 == 3 )
            TEST_ASSERT( mat2.e21 == 4 )
            TEST_ASSERT( mat2.e22 == 5 )
            TEST_ASSERT( mat2.e23 == 6 )
            TEST_ASSERT( mat2.e31 == 7 )
            TEST_ASSERT( mat2.e32 == 8 )
            TEST_ASSERT( mat2.e33 == 9 )

            // from quaternions
            Matrix3 mat3( Quaternion::sIdentity );
            TEST_ASSERT( mat3 == Matrix3::sIdentity );

            Quaternion quatRotate180y( 0, 0, 1, 0 );
            TEST_ASSERT( quatRotate180y * Vector3( 0, 0, 2 ) == Vector3( 0, 0, -2 ) );
            Matrix3 mat3rotate180y = Matrix3( quatRotate180y );
            TEST_ASSERT( mat3rotate180y * Vector3( 0, 0, 2 ) == Vector3( 0, 0, -2 ) );
        }

        void TestDeterminant()
        {
            Matrix3 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat.Determinant() == 0 )
            Matrix3 mat2( -4, 12, 34, -5, 6, 7, 3, -8, 17 );
            TEST_ASSERT( mat2.Determinant() == 1388 )
        }

        void TestComparison()
        {
            Matrix3 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat == mat );
            Matrix3 mat2;
            mat2 = Matrix3( 42, 2, 3, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 42, 3, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 42, 4, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 42, 5, 6, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 4, 42, 6, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 4, 5, 42, 7, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 4, 5, 6, 42, 8, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 4, 5, 6, 7, 42, 9 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix3( 1, 2, 3, 4, 5, 6, 7, 8, 42 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
        }

        void TestAddition()
        {
            Matrix3 mat( 1, 2, 3,
                         4, 5, 6,
                         7, 8, 9 );
            Matrix3 mat2( 10, 20, 30,
                          40, 50, 60,
                          70, 80, 90 );
            TEST_ASSERT( mat + mat2 == Matrix3( 11, 22, 33, 44, 55, 66, 77, 88, 99 ) );
            mat += mat2;
            TEST_ASSERT( mat == Matrix3( 11, 22, 33, 44, 55, 66, 77, 88, 99 ) );
        }

        void TestSubtraction()
        {
            Matrix3 mat( 1, 2, 3,
                         4, 5, 6,
                         7, 8, 9 );
            Matrix3 mat2( 11, 22, 33,
                          44, 55, 66,
                          77, 88, 99 );
            TEST_ASSERT( mat2 - mat == Matrix3( 10, 20, 30, 40, 50, 60, 70, 80, 90 ) );
            mat2 -= mat;
            TEST_ASSERT( mat2 == Matrix3( 10, 20, 30, 40, 50, 60, 70, 80, 90 ) );
            TEST_ASSERT( -mat == Matrix3( -1, -2, -3, -4, -5, -6, -7, -8, -9 ) )
            TEST_ASSERT( mat - mat == Matrix3::sZero )
        }

        void TestMultiplication()
        {
            // scalar multiplication
            Matrix3 mat( 2, 4, 6, 8, 10, 12, 14, 16, 18 );
            TEST_ASSERT( mat * 2 == Matrix3( 4, 8, 12, 16, 20, 24, 28, 32, 36 ) );
            TEST_ASSERT( mat * FIXED( 0.5 ) == Matrix3( 1, 2, 3, 4, 5, 6, 7, 8, 9 ) );
            TEST_ASSERT( 2 * mat == mat * 2 )
            TEST_ASSERT( ( Matrix3( mat ) *= 2 ) == 2 * mat );


            // matrix multiplication
            Matrix3 mat2( 1, -2, 3, -4, 5, -6, 7, -8, 9 );
            TEST_ASSERT( mat * mat2 == Matrix3( 28, -32, 36, 52, -62, 72, 76, -92, 108 ) )

            Matrix3 mat3( 1, 3, 6, 9, 12, 15, 18, 21, 24 );
            TEST_ASSERT( mat * mat3 == Matrix3( 146, 180, 216, 314, 396, 486, 482, 612, 756 ) )

            TEST_ASSERT( mat * mat2 != mat2 * mat )
            TEST_ASSERT( mat * mat3 != mat3 * mat )
            TEST_ASSERT( mat * ( mat2 * mat3 ) == ( mat * mat2 ) * mat3 )
        }

        void TestDivision()
        {
            Matrix3 mat( 2, 4, 6, 8, 10, 12, 14, 16, 18 );
            TEST_ASSERT( mat / 2 == Matrix3( 1, 2, 3, 4, 5, 6, 7, 8, 9 ) );
            TEST_ASSERT( mat / FIXED( 0.5 ) == Matrix3( 4, 8, 12, 16, 20, 24, 28, 32, 36 ) );
        }

        void TestTranspose()
        {
            Matrix3 mat( 1, 2, 3,
                         4, 5, 6,
                         7, 8, 9 );
            Matrix3 mat2( 10, 11, 12,
                          13, 14, 15,
                          16, 17, 18 );
            Matrix3 matT( 1, 4, 7,
                          2, 5, 8,
                          3, 6, 9 );
            TEST_ASSERT( mat.Transpose() == matT );
            TEST_ASSERT( matT.Transpose() == mat );
            TEST_ASSERT( mat.Transpose().Transpose() == mat );
            TEST_ASSERT( Matrix3::sIdentity.Transpose() == Matrix3::sIdentity )
            TEST_ASSERT( ( 2 * mat ).Transpose() == 2 * mat.Transpose() );
            TEST_ASSERT( ( mat * mat2 ).Transpose() == mat2.Transpose() * mat.Transpose() );
            TEST_ASSERT( ( mat + mat2 ).Transpose() == mat.Transpose() + mat2.Transpose() );
            TEST_ASSERT( mat.Determinant() == mat.Transpose().Determinant() );
        }

        void TestInverse()
        {
            Matrix3 mat( 3, 1, 1, 3, 2, 1, 2, 1, 2 );
            TEST_ASSERT( mat.Determinant() != 0 )
            TEST_ASSERT( mat.Inverse() * mat == mat * mat.Inverse() )
            TEST_ASSERT( mat.Inverse() * mat == Matrix3::sIdentity )
            TEST_ASSERT( mat * mat.Inverse() == Matrix3::sIdentity )
            TEST_ASSERT( Matrix3::sIdentity.Inverse() == Matrix3::sIdentity )

            Matrix3 mat2( 1, 2, 3, 0, 1, 4, 5, 6, 0 );
            TEST_ASSERT( ( mat * mat2 ).Inverse() == mat2.Inverse() * mat.Inverse() )
        }

        void TestToQuaternion()
        {
            Quaternion q  = Quaternion::AngleAxis( 90, Vector3::sUp );
            Matrix3    m( q );
            Quaternion q2 = m.ToQuaternion();
            TEST_ASSERT( Fixed::IsFuzzyZero( ( q - q2 ).Magnitude() ) )

            q  = Quaternion::AngleAxis( 42, Vector3( 4, -7, 3 ).Normalized() );
            m  = Matrix3( q );
            q2 = m.ToQuaternion();
            TEST_ASSERT( Fixed::IsFuzzyZero( ( q - q2 ).Magnitude() ) )
        }

        void TestVectorMultiplication()
        {
            Matrix3 mat( 1, 2, 3, 0, 1, 4, 5, 6, 0 );
            Vector3 vec( 4, 5, 6 );
            TEST_ASSERT( mat * vec == Vector3( 32, 29, 50 ) )
            TEST_ASSERT( mat.Inverse() * ( mat * vec ) == vec )
        }

        void TestSkewSymmetric()
        {
            Vector3 v1( 1, 2, 3 );
            Vector3 v2( 4, 5, 6 );
            Matrix3 m = Matrix3::SkewSymmetric( v1 );
            TEST_ASSERT( Vector3::Cross( v1, v2 ) == m * v2 );

            v1 = Vector3( -4, 5, -9 );
            v2 = Vector3( 2, -7, 41 );
            m  = Matrix3::SkewSymmetric( v1 );
            TEST_ASSERT( Vector3::Cross( v1, v2 ) == m * v2 );
        }
    };
}