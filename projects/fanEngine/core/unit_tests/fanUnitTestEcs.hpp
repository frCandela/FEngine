#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct TestEcsSingleton : public EcsSingleton
    {

    ECS_SINGLETON( TestEcsSingleton )
        static void SetInfo( EcsSingletonInfo& /*_info*/ )
        {
        }
        static void Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
        {
            using namespace test;
            TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
            testSingleton.mValueInt = 0;
        }
        static void SetValueInt( EcsSingleton& _singleton, int _value )
        {
            using namespace test;
            TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
            testSingleton.mValueInt      = _value;
        }
        int mValueInt = 0;
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    struct TestEcsComponent : public EcsComponent
    {
    ECS_COMPONENT( TestEcsComponent )
        static void SetInfo( EcsComponentInfo& /*_info*/ )
        {
        }
        static void Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            using namespace test;
            TestComponent& testComponent = static_cast<TestComponent&>(_component);
            testComponent.mValueInt = 0;
        }
        int mValueInt = 0;
    };

    struct TagTest : EcsTag
    {
    ECS_TAG( TagTest )
    };
    struct TagTest2 : EcsTag
    {
    ECS_TAG( TagTest2 )
    };

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestEcs : public UnitTest<UnitTestEcs>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestEcs::TestAddTagType,          "tag add types " },
                     { &UnitTestEcs::TestAddRemoveTags,       "tag add/remove " },
                     { &UnitTestEcs::TestFaultyAddRemoveTags, "tag multiple add/remove" },
            };
        }
        void Create() override
        {
            mWorld.AddComponentType<TestEcsComponent>();
            mWorld.AddSingletonType<TestEcsSingleton>();
            mWorld.AddTagType<TagTest>();
            mWorld.InitSingletons();
            mWorld.PostInitSingletons();
        }
        void Destroy() override {}

        EcsWorld mWorld;

        void TestAddTagType()
        {
            EcsWorld world;
            TEST_ASSERT( world.NumTags() == 0 );
            TEST_ASSERT( world.GetFistTagIndex() == ecsSignatureLength );
            world.AddTagType<TagTest>();
            TEST_ASSERT( world.NumTags() == 1 );
            TEST_ASSERT( world.GetFistTagIndex() == ecsSignatureLength - 1 );
            TEST_ASSERT( world.IndexedGetTagInfo( world.GetFistTagIndex() ).mName == "TagTest" );
            world.AddTagType<TagTest2>();
            TEST_ASSERT( world.NumTags() == 2 );
            TEST_ASSERT( world.GetFistTagIndex() == ecsSignatureLength - 2 );
            TEST_ASSERT( world.IndexedGetTagInfo( world.GetFistTagIndex() ).mName == "TagTest2" );
        }

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

            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
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
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            // fresh entity
            entity = mWorld.CreateEntity();
            handle = mWorld.AddHandle( entity );
            mWorld.AddComponent<TestEcsComponent>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );
            mWorld.AddTag<TagTest>( entity );
            mWorld.RemoveTag<TagTest>( entity );
            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

            mWorld.ApplyTransitions();
            entity = mWorld.GetEntity( handle );

            TEST_ASSERT( !mWorld.HasTag<TagTest>( entity ) );

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
    };
}