#pragma once

#include "core/unit_tests/fanUnitTest.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsComponent.hpp"
#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    namespace test
    {
        //====================================================================================================
        //====================================================================================================
        struct TestSingleton : public EcsSingleton
        {
        ECS_SINGLETON( TestSingleton )
            static void SetInfo( EcsSingletonInfo& )
            {
            }
            static void Init( EcsWorld& /*_world*/, EcsSingleton& _singleton )
            {
                TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
                testSingleton.mValueInt = 0;
            }
            static void SetValueInt( EcsSingleton& _singleton, int _value )
            {
                TestSingleton& testSingleton = static_cast<TestSingleton&>(_singleton);
                testSingleton.mValueInt      = _value;
            }
            int mValueInt = 0;
        };

        //====================================================================================================
        //====================================================================================================
        struct TestComponent : public EcsComponent
        {
        ECS_COMPONENT( TestComponent )
            static void SetInfo( EcsComponentInfo& )
            {
            }
            static void Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
            {
                TestComponent& testComponent = static_cast<TestComponent&>(_component);
                testComponent.mValueInt   = 0;
                testComponent.mValueFloat = 0;
            }
            static void SetValueInt( EcsComponent& _component, int _value )
            {
                TestComponent& testComponent = static_cast<TestComponent&>(_component);
                testComponent.mValueInt      = _value;
            }
            static void SetValueFloat( EcsComponent& _component, float _value )
            {
                TestComponent& testComponent = static_cast<TestComponent&>(_component);
                testComponent.mValueFloat    = _value;
            }
            static void DoNothing( EcsComponent& _component )
            {
                (void)_component;
            }

            int   mValueInt   = 0;
            float mValueFloat = 0;
        };
    }

    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    class UnitTestSignal : public UnitTest<UnitTestSignal>
    {
    public:

        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestSignal::TestConnect,             "Connect " },
            };
        }
        void Create() override
        {
            using namespace test;
            mWorld.AddComponentType<TestComponent>();
            mWorld.AddSingletonType<TestSingleton>();
            mWorld.InitSingletons();
            mWorld.PostInitSingletons();
        }

        void Destroy() override {}

        EcsWorld mWorld;

        void TestConnect()
        {
            struct TestStruct
            {
                void SetValue( int _value ) { mValue = _value; }
                int mValue = 0;
            };

            TestStruct testStruct1;
            testStruct1.mValue = 1;
            TestStruct testStruct2;
            testStruct2.mValue = 2;

            Signal<int> signal;
            signal.Connect( &TestStruct::SetValue, &testStruct1 );
            signal.Connect( &TestStruct::SetValue, &testStruct2 );
            signal.Emmit( 3 );
            TEST_ASSERT( testStruct1.mValue == 3 && testStruct2.mValue == 3 )
            signal.Disconnect( (size_t)&testStruct1 );
            signal.Emmit( 4 );
            TEST_ASSERT( testStruct1.mValue == 3 );
            TEST_ASSERT( testStruct2.mValue == 4 );
            signal.Clear();
            signal.Emmit( 5 );
            TEST_ASSERT( testStruct2.mValue == 4 );
        }
    };
}