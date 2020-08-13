#pragma once

#include "core/fanUnitTest.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "scene/components/ui/fanUIAlign.hpp"
#include "scene/components/ui/fanUITransform.hpp"
#include "scene/components/ui/fanUILayout.hpp"
#include "scene/systems/fanAlignUI.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    class UnitTestUIAlign : public UnitTest<UnitTestUIAlign>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestUIAlign::TestAlignCornersNoOffset, "Align corners no offset" },
                     { &UnitTestUIAlign::TestAlignOffsetRatio,     "Align offset ratio" },
                     { &UnitTestUIAlign::TestAlignOffsetPixels,    "Align offset pixels" },
            };
        }
        void Create() override
        {
            mWorld.AddComponentType<UITransform>();
            mWorld.AddComponentType<UIAlign>();
            mWorld.AddComponentType<UILayout>();
            EcsEntity entityRoot  = mWorld.CreateEntity();
            EcsEntity entityChild = mWorld.CreateEntity();
            mHandleRoot  = mWorld.AddHandle( entityRoot );
            mHandleChild = mWorld.AddHandle( entityChild );
            mWorld.AddComponent<UITransform>( entityRoot );
            mWorld.AddComponent<UITransform>( entityChild );
            mWorld.AddComponent<UIAlign>( entityChild );
            mWorld.ApplyTransitions();
            mRootTransform = &mWorld.GetComponent<UITransform>( mWorld.GetEntity(mHandleRoot));
            mChildTransform = &mWorld.GetComponent<UITransform>( mWorld.GetEntity(mHandleChild));
            mChildAlign = &mWorld.GetComponent<UIAlign>( mWorld.GetEntity(mHandleChild));
            mChildAlign->mParent.Create(mHandleRoot);

            mRootTransform->mPosition   = {10,20};
            mRootTransform->mSize       = {50,100};
            mChildTransform->mPosition  = {1000,1000};
            mChildTransform->mSize      = {20,30};

        }
        void Destroy() override {}

        EcsWorld  mWorld;
        EcsHandle mHandleRoot;
        EcsHandle mHandleChild;
        UITransform* mRootTransform;
        UITransform* mChildTransform;
        UIAlign* mChildAlign;

        void TestAlignCornersNoOffset()
        {
            TEST_ASSERT(mChildAlign->mOffset == glm::vec2(0.f,0.f) );

            mChildAlign->mCorner = UIAlign::TopLeft;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(10,20) );

            mChildAlign->mCorner = UIAlign::TopRight;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(40,20) );

            mChildAlign->mCorner = UIAlign::BottomLeft;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(10,90) );

            mChildAlign->mCorner = UIAlign::BottomRight;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(40,90) );
        }

        void TestAlignOffsetRatio()
        {
            mChildAlign->mUnitType = UIAlign::Ratio;

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::Horizontal;
            mChildAlign->mOffset.x = 0.5f;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(25,20) );

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::Vertical;
            mChildAlign->mOffset.y = 0.5f;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(10,55) );

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::HorizontalVertical;
            mChildAlign->mOffset = glm::vec2( 0.5f, 0.5f );
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(25,55) );
        }

        void TestAlignOffsetPixels()
        {
            mChildAlign->mUnitType = UIAlign::Pixels;

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::Horizontal;
            mChildAlign->mOffset.x = 1.f;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(11,20) );

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::Vertical;
            mChildAlign->mOffset.y = 2.f;
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(10,22) );

            mChildAlign->mCorner = UIAlign::TopLeft;
            mChildAlign->mDirection = UIAlign::HorizontalVertical;
            mChildAlign->mOffset = glm::vec2( 1.f, 2.f );
            mWorld.Run<SAlignUI>();
            TEST_ASSERT( mChildTransform->mPosition == glm::ivec2(11,22) );
        }
    };
}