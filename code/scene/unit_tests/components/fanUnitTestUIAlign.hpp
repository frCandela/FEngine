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
            return { { &UnitTestUIAlign::TestAlignCornersNoRatio,  "Align corners no ratio" },
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

        void TestAlignCornersNoRatio()
        {
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
    };
}