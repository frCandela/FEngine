#pragma once

#include "core/fanUnitTest.hpp"
#include "ecs/fanSlot.hpp"
#include "ecs/fanEcsWorld.hpp"
#include "ecs/fanEcsComponent.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct TestComponent : public EcsComponent
    {
        ECS_COMPONENT( TestComponent )
        static void SetInfo( EcsComponentInfo& _info )
        {
            _info.name = "test";
            _info.mSlots.push_back( new Slot<int>( "test int", &TestComponent::SetValueInt ) );
            _info.mSlots.push_back( new Slot<float>( "test float", &TestComponent::SetValueFloat ) );
        }
        static void	Init( EcsWorld& /*_world*/, EcsEntity /*_entity*/, EcsComponent& _component )
        {
            TestComponent& testComponent = static_cast<TestComponent&>(_component);
            testComponent.mValueInt = 0;
            testComponent.mValueFloat = 0;
        }
        static void SetValueInt( EcsComponent& _component, int _value )
        {
            TestComponent& testComponent = static_cast<TestComponent&>(_component);
            testComponent.mValueInt = _value;
        }
        static void SetValueFloat( EcsComponent& _component,float _value )
        {
            TestComponent& testComponent = static_cast<TestComponent&>(_component);
            testComponent.mValueFloat = _value;
        }

        int     mValueInt = 0;
        float   mValueFloat = 0;
    };


    //========================================================================================================
    //========================================================================================================
    class UnitTestSignal : public UnitTest<UnitTestSignal>
    {
    public:
        static std::vector<TestMethod> GetTests()
        {
            return { { &UnitTestSignal::TestConnect, "Connect " },
                     { &UnitTestSignal::TestReceiver, "Receiver" },
            };
        }
        void Create() override {}
        void Destroy() override {}

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

        void TestReceiver()
        {
            EcsWorld world;
            world.AddComponentType<TestComponent>();
            EcsEntity entity = world.CreateEntity();
            world.AddComponent<TestComponent>(entity);
            EcsHandle handle = world.AddHandle( entity );

            TestComponent& testComponent = world.AddComponent<TestComponent>(entity);

            SlotBase* slotInt = new Slot<int>( "test int", &TestComponent::SetValueInt );

            Signal<int>     signalInt;
            signalInt.Connect( world, handle, TestComponent::Info::s_type, slotInt );
            TEST_ASSERT( testComponent.mValueInt == 0 );
            signalInt.Emmit(12);
            TEST_ASSERT( testComponent.mValueInt == 12 );
            signalInt.Disconnect( handle );
            signalInt.Emmit(25);
            TEST_ASSERT( testComponent.mValueInt == 12 );

            SlotBase* slotFloat = new Slot<float>( "test float", &TestComponent::SetValueFloat );
            Signal<float>   signalFloat;
            signalFloat.Connect( world, handle, TestComponent::Info::s_type, slotFloat );
            TEST_ASSERT( testComponent.mValueFloat == 0 );
            signalFloat.Emmit(13.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );

            signalFloat.Clear();
            signalFloat.Emmit(14.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );

            delete slotFloat;
            delete slotInt;
        }
    };
}