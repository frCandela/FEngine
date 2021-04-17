#pragma once

#include "core/fanAssert.hpp"
#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanMatrix4.hpp"
#include "core/math/fanQuaternion.hpp"

namespace fan
{
    //======================================================================================================================
    //======================================================================================================================
    class UnitTestMatrix4 : public UnitTest<UnitTestMatrix4>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestMatrix4::TestConstructors,         "constructors" },
                     { &UnitTestMatrix4::TestComparison,           "comparison" },
                     { &UnitTestMatrix4::TestAddition,             "addition" },
                     { &UnitTestMatrix4::TestSubtraction,          "subtraction" },
                     { &UnitTestMatrix4::TestMultiplication,       "multiplication" },
                     { &UnitTestMatrix4::TestDivision,             "division" },
                     { &UnitTestMatrix4::TestDeterminant,          "determinant" },
                     { &UnitTestMatrix4::TestTranspose,            "transpose" },
                     { &UnitTestMatrix4::TestInverse,              "inverse" },
                     { &UnitTestMatrix4::TestVectorMultiplication, "vector multiplication" },
                     { &UnitTestMatrix4::TestTransforms,           "tranforms" },
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
            Matrix4 mat;
            TEST_ASSERT( mat.e11 == 0 )
            TEST_ASSERT( mat.e12 == 0 )
            TEST_ASSERT( mat.e13 == 0 )
            TEST_ASSERT( mat.e14 == 0 )

            TEST_ASSERT( mat.e21 == 0 )
            TEST_ASSERT( mat.e22 == 0 )
            TEST_ASSERT( mat.e23 == 0 )
            TEST_ASSERT( mat.e14 == 0 )

            TEST_ASSERT( mat.e31 == 0 )
            TEST_ASSERT( mat.e32 == 0 )
            TEST_ASSERT( mat.e33 == 0 )
            TEST_ASSERT( mat.e14 == 0 )

            Matrix4 mat2( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2.e11 == 1 )
            TEST_ASSERT( mat2.e12 == 2 )
            TEST_ASSERT( mat2.e13 == 3 )
            TEST_ASSERT( mat2.e14 == 4 )

            TEST_ASSERT( mat2.e21 == 5 )
            TEST_ASSERT( mat2.e22 == 6 )
            TEST_ASSERT( mat2.e23 == 7 )
            TEST_ASSERT( mat2.e24 == 8 )

            TEST_ASSERT( mat2.e31 == 9 )
            TEST_ASSERT( mat2.e32 == 10 )
            TEST_ASSERT( mat2.e33 == 11 )
            TEST_ASSERT( mat2.e34 == 12 )

            // from quaternions
            Matrix4 mat3( Quaternion::sIdentity, Vector3( 0, 0, 0 ) );
            TEST_ASSERT( mat3 == Matrix4::sIdentity );

            Quaternion quatRotate180y( 0, 0, 1, 0 );
            TEST_ASSERT( quatRotate180y * Vector3(0,0,2) == Vector3(0,0,-2) );
            Matrix4 mat3rotate180y = Matrix4( quatRotate180y, Vector3::sZero );
            TEST_ASSERT( mat3rotate180y * Vector3(0,0,2) == Vector3(0,0,-2) );
            Matrix4 mat3rotate180yTranslateUp = Matrix4( quatRotate180y, Vector3::sUp );
            TEST_ASSERT( mat3rotate180yTranslateUp * Vector3(0,0,2) == Vector3(0,0,-2)+ Vector3::sUp );
        }

        void TestDeterminant()
        {
            Matrix4 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat.Determinant() == 0 )
            Matrix4 mat2( -4, 12, 34, -5, 6, 7, 3, -8, 17, 1, -6, 3 );
            TEST_ASSERT( mat2.Determinant() == -2618 )
            Matrix4 mat3( 12, -4, 3, -8, 17, 6, 34, 1, -6, 3, -5, 7 );
            TEST_ASSERT( mat3.Determinant() == -847 )
        }

        void TestComparison()
        {
            Matrix4 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat == mat );
            Matrix4 mat2;
            mat2 = Matrix4( 42, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 42, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 42, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 42, 5, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 42, 6, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 42, 7, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 42, 8, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 7, 42, 9, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 42, 10, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 9, 42, 11, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 42, 12 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
            mat2 = Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 42 );
            TEST_ASSERT( mat2 != mat );
            TEST_ASSERT( mat2 == mat2 );
        }

        void TestAddition()
        {
            Matrix4 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13 );
            Matrix4 mat2( 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 );
            TEST_ASSERT( mat + mat2 == Matrix4( 11, 22, 33, 44, 55, 66, 77, 88, 99, 111, 122, 133 ) );
            mat += mat2;
            TEST_ASSERT( mat == Matrix4( 11, 22, 33, 44, 55, 66, 77, 88, 99, 111, 122, 133 ) );
        }

        void TestSubtraction()
        {
            Matrix4 mat( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 );
            Matrix4 mat2( 11, 22, 33, 44, 55, 66, 77, 88, 99, 110, 121, 132 );
            TEST_ASSERT( mat2 - mat == Matrix4( 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 ) );
            mat2 -= mat;
            TEST_ASSERT( mat2 == Matrix4( 10, 20, 30, 40, 50, 60, 70, 80, 90, 100, 110, 120 ) );
            TEST_ASSERT( -mat == Matrix4( -1, -2, -3, -4, -5, -6, -7, -8, -9, -10, -11, -12 ) )
            TEST_ASSERT( mat - mat == Matrix4::sZero )
        }

        void TestMultiplication()
        {
            // scalar multiplication
            Matrix4 mat( 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24 );
            TEST_ASSERT( mat * 2 == Matrix4( 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48 ) );
            TEST_ASSERT( mat * FIXED( 0.5 ) == Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ) );
            TEST_ASSERT( 2 * mat == mat * 2 )
            TEST_ASSERT( ( Matrix4( mat ) *= 2 ) == 2 * mat );

            // matrix multiplication
            Matrix4 mat2( 1, -2, 3, -4, 5, -6, 7, -8, 9, -10, 11, -12 );
            TEST_ASSERT( mat * mat2 == Matrix4( 76, -88, 100, -104, 196, -232, 268, -288, 316, -376, 436, -472 ) )

            Matrix4 mat3( 1, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33 );
            TEST_ASSERT( mat * mat3 == Matrix4( 194, 228, 264, 308, 490, 588, 696, 820, 786, 948, 1128, 1332 ) )

            TEST_ASSERT( mat * mat2 != mat2 * mat )
            TEST_ASSERT( mat * mat3 != mat3 * mat )
            TEST_ASSERT( mat * ( mat2 * mat3 ) == ( mat * mat2 ) * mat3 )
        }

        void TestDivision()
        {
            Matrix4 mat( 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24 );
            TEST_ASSERT( mat / 2 == Matrix4( 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12 ) );
            TEST_ASSERT( mat / FIXED( 0.5 ) == Matrix4( 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48 ) );
        }

        void TestTranspose()
        {
            Matrix4 mat( 1, 2, 3, 4,
                         5, 6, 7, 8,
                         9, 10, 11, 12 );
            Matrix4 mat2( 13, 14, 15, 16,
                          17, 18, 19, 20,
                          21, 22, 23, 24 );
            Matrix4 matT( 1, 5, 9, 0,
                          2, 6, 10, 0,
                          3, 7, 11, 0 );
            TEST_ASSERT( mat.Transpose() == matT );
            TEST_ASSERT( Matrix4::sIdentity.Transpose() == Matrix4::sIdentity )
            TEST_ASSERT( ( 2 * mat ).Transpose() == 2 * mat.Transpose() );
            TEST_ASSERT( ( mat * mat2 ).Transpose() == mat2.Transpose() * mat.Transpose() );
            TEST_ASSERT( ( mat + mat2 ).Transpose() == mat.Transpose() + mat2.Transpose() );
            TEST_ASSERT( mat.Determinant() == mat.Transpose().Determinant() );
        }

        void TestInverse()
        {
            Matrix4 mat( 3, 1, 1, 1,
                         3, 2, 1, 2,
                         2, 1, 1, 3 );
            Matrix4 invMat( 1, 0, -1, 2,
                            -1, 1, 0, -1,
                            -1, -1, 3, -6 );

            TEST_ASSERT( mat.Determinant() != 0 )
            TEST_ASSERT( mat.Inverse() == invMat )
            TEST_ASSERT( mat.Inverse() * mat == mat * mat.Inverse() )
            TEST_ASSERT( mat.Inverse() * mat == Matrix4::sIdentity )
            TEST_ASSERT( mat * mat.Inverse() == Matrix4::sIdentity )
            TEST_ASSERT( Matrix4::sIdentity.Inverse() == Matrix4::sIdentity )

            Matrix4 mat2( 3, 3, 2, 4,
                          1, 2, 1, 5,
                          1, 1, 1, 6 );
            TEST_ASSERT( ( mat * mat2 ).Inverse() == mat2.Inverse() * mat.Inverse() )
        }

        void TestVectorMultiplication()
        {
            Matrix4 mat( 3, 3, 2, 4,
                         1, 2, 1, 5,
                         1, 1, 1, 6 );
            Vector3 vec( 13, 14, 15 );
            TEST_ASSERT( mat * vec == Vector3( 115, 61, 48 ) )
            TEST_ASSERT( mat.Inverse() * ( mat * vec ) == vec )
        }

        void TestTransforms()
        {
            // inverse transform
            Matrix4 rotationY90translate( 0, 0, 1, 4,
                                          0, 1, 0, 5,
                                          -1, 0, 0, 6 );
            Vector3 vec( 8, -14, 21 );
            TEST_ASSERT( rotationY90translate * vec == Vector3( 25, -9, -2 ) )
            TEST_ASSERT( rotationY90translate.InverseTransform( rotationY90translate * vec ) == vec )

            Matrix4   rotationX90Y90Z90translate( 0, 0, 1, 4,
                                                  0, -1, 0, 5,
                                                  1, 0, 0, 9 );
            glm::vec3 v = Math::ToGLM( rotationX90Y90Z90translate * vec );
            TEST_ASSERT( rotationX90Y90Z90translate * vec == Vector3( 25, 19, 17 ) )
            TEST_ASSERT( rotationX90Y90Z90translate.InverseTransform( rotationX90Y90Z90translate * vec ) == vec )


            // transform direction
            Matrix4 rotationY90( 0, 0, 1, 0,
                                 0, 1, 0, 0,
                                 -1, 0, 0, 0 );
            TEST_ASSERT( rotationY90translate.TransformDirection( vec ) == rotationY90 * vec )

            Matrix4 rotationX90Y90Z90( 0, 0, 1, 0,
                                       0, -1, 0, 0,
                                       1, 0, 0, 0 );
            TEST_ASSERT( rotationX90Y90Z90translate.TransformDirection( vec ) == rotationX90Y90Z90 * vec )

            // inverse transform direction
            TEST_ASSERT( rotationY90translate.InverseTransformDirection( vec ) == rotationY90.InverseTransform( vec ) )
            TEST_ASSERT( rotationX90Y90Z90translate.InverseTransformDirection( vec ) == rotationX90Y90Z90.InverseTransform( vec ) )
            TEST_ASSERT( rotationY90translate.InverseTransformDirection( rotationY90translate.TransformDirection( vec ) ) == vec )
            TEST_ASSERT( rotationX90Y90Z90translate.InverseTransformDirection( rotationX90Y90Z90translate.TransformDirection( vec ) ) == vec )
        }
    };
}