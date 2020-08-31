#pragma once

#include "core/fanUnitTest.hpp"
#include "ecs/fanSlot.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct TestEcsSingleton : public EcsSingleton
    {
        ECS_SINGLETON( TestEcsSingleton );
        static void SetInfo( EcsSingletonInfo& _info )
        {
            _info.mName = "test singleton";
        }
        static void	Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
            testSingleton.mValueInt = 0;
        }
        static void SetValueInt( EcsSingleton& _singleton, int _value )
        {
            TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
            testSingleton.mValueInt = _value;
        }
        int     mValueInt = 0;
    };

    //========================================================================================================
    //========================================================================================================
    struct TestEcsComponent : public EcsComponent
    {
        ECS_COMPONENT( TestEcsComponent )
        static void SetInfo( EcsComponentInfo& _info )
        {
            _info.mName = "test";
        }
        static void	Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            TestComponent& testComponent = static_cast<TestComponent&>(_component);
            testComponent.mValueInt = 0;
        }
        int     mValueInt = 0;
    };

    struct TagTest  : EcsTag  { ECS_TAG( TagTest )  };

    //========================================================================================================
    //========================================================================================================
    class UnitTestEcs : public UnitTest<UnitTestEcs>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestEcs::TestAddRemoveTags,          "test add remove tags " },
                     { &UnitTestEcs::TestFaultyAddRemoveTags,    "test faulty add remove tags " },
                     { &UnitTestEcs::TestDuplicateAddRemoveTags, "test duplicate add remove tags " },
            };
        }
        void Create() override
        {
            mWorld.AddComponentType<TestEcsComponent>();
            mWorld.AddSingletonType<TestEcsSingleton>();
            mWorld.AddTagType<TagTest>();
        }
        void Destroy() override {}

        EcsWorld mWorld;

        void TestAddRemoveTags()
        {
            EcsEntity entity = mWorld.CreateEntity();
            EcsHandle handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            mWorld.AddTag<TagTest>( entity );
            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );
        }

        void TestFaultyAddRemoveTags()
        {
            EcsEntity entity = mWorld.CreateEntity();
            EcsHandle handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );

            // fresh entity
            entity = mWorld.CreateEntity();
            handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );

            // fresh entity
            entity = mWorld.CreateEntity();
            handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            mWorld.AddTag<TagTest>( entity );
            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );
        }

        void TestDuplicateAddRemoveTags()
        {
            EcsEntity entity = mWorld.CreateEntity();
            EcsHandle handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );


            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );
        }
            /*
            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( ! mWorld.HasTag<TagTest>( entity ) );
            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            mWorld.AddTag<TagTest>( entity );
            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( mWorld.HasTag<TagTest>( entity ) );
        }*/
    };
}