#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/math/fanMathUtils.hpp"
#include "render/fanWindow.hpp"
#include "engine/components/fanFxTransform.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestFxTransform : public UnitTest<UnitTestFxTransform>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestFxTransform::TestInit,               "init" },
                     { &UnitTestFxTransform::TestDirections,         "directions" },
                     { &UnitTestFxTransform::TestTransformPoint,     "transform point" },
                     { &UnitTestFxTransform::TestTransformDirection, "transform direction" },
                     { &UnitTestFxTransform::TestLookAt,             "look at" },
            };
        }

        FxTransform mTransform;

        void Create() override
        {
            EcsWorld mWorld;
            FxTransform::Init( mWorld, EcsEntity(), mTransform );
        }
        void Destroy() override {}

        void TestInit()
        {
            TEST_ASSERT( mTransform.mPosition == Vector3::sZero )
            TEST_ASSERT( mTransform.mRotation == Quaternion::sIdentity )
            TEST_ASSERT( mTransform.mScale == Vector3::sOne )
        }

        void TestDirections()
        {
            TEST_ASSERT( mTransform.Left() == Vector3::sLeft )
            TEST_ASSERT( mTransform.Right() == Vector3::sRight )
            TEST_ASSERT( mTransform.Forward() == Vector3::sForward )
            TEST_ASSERT( mTransform.Back() == Vector3::sBack )
            TEST_ASSERT( mTransform.Up() == Vector3::sUp )
            TEST_ASSERT( mTransform.Down() == Vector3::sDown )

            mTransform.mPosition = Vector3( 1, 2, 3 );
            mTransform.mRotation = Quaternion::Euler( 0, 90, 0 );
            mTransform.mScale    = { 2, 2, 2 };

            TEST_ASSERT( mTransform.Left() == Vector3::sBack )
            TEST_ASSERT( mTransform.Right() == Vector3::sForward )
            TEST_ASSERT( mTransform.Forward() == Vector3::sLeft )
            TEST_ASSERT( mTransform.Back() == Vector3::sRight )
            TEST_ASSERT( mTransform.Up() == Vector3::sUp )
            TEST_ASSERT( mTransform.Down() == Vector3::sDown )
        }

        void TestTransformPoint()
        {
            Vector3 point = { 1, 2, 3 };

            TEST_ASSERT( mTransform.TransformPoint( point ) == point )
            TEST_ASSERT( mTransform.InverseTransformPoint( mTransform.TransformPoint( point ) ) == point )

            mTransform.mPosition = Vector3::sForward;
            TEST_ASSERT( mTransform.TransformPoint( point ) == point + Vector3::sForward )
            TEST_ASSERT( mTransform.InverseTransformPoint( mTransform.TransformPoint( point ) ) == point )

            mTransform.mScale = Vector3::sOne * 2;
            TEST_ASSERT( mTransform.TransformPoint( point ) == Vector3( 2, 4, 7 ) )
            TEST_ASSERT( mTransform.InverseTransformPoint( mTransform.TransformPoint( point ) ) == point )

            mTransform.mRotation = Quaternion::Euler( 0, 90, 0 );
            TEST_ASSERT( mTransform.TransformPoint( point ) == Vector3( 6, 4, -1 ) )
            TEST_ASSERT( mTransform.InverseTransformPoint( mTransform.TransformPoint( point ) ) == point )
        }

        void TestTransformDirection()
        {
            Vector3 point = { 1, 2, 3 };

            TEST_ASSERT( mTransform.TransformDirection( point ) == point )

            mTransform.mPosition = Vector3::sForward;
            TEST_ASSERT( mTransform.TransformDirection( point ) == point )

            mTransform.mScale = Vector3::sOne * 2;
            TEST_ASSERT( mTransform.TransformDirection( point ) == point )

            mTransform.mRotation = Quaternion::Euler( 0, 90, 0 );
            TEST_ASSERT( mTransform.TransformDirection( point ) == Vector3( 3, 2, -1 ) )
            TEST_ASSERT( mTransform.InverseTransformDirection( mTransform.TransformDirection( point ) ) == point )
        }

        void TestLookAt()
        {
            mTransform.mPosition = Vector3::sLeft;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sRight ) )

            mTransform.mPosition = Vector3::sRight;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sLeft ) )

            mTransform.mPosition = Vector3::sDown;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sUp ) )

            mTransform.mPosition = Vector3::sUp;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sDown ) )

            mTransform.mPosition = Vector3::sBack;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sForward ) )

            mTransform.mPosition = Vector3::sForward;
            mTransform.LookAt( Vector3::sZero );
            TEST_ASSERT( Vector3::IsFuzzyZero( mTransform.TransformDirection( Vector3::sForward ) - Vector3::sBack ) )
        }
    };
}