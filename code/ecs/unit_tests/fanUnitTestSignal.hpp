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
            return { { &UnitTestSignal::TestConnect,    "Connect " },
                     { &UnitTestSignal::TestSlotPtr,    "Slot ptr" },
                     { &UnitTestSignal::TestSignalSlot, "Signal slot" },
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

        void TestSlotPtr()
        {
            EcsWorld * world = nullptr;
            EcsHandle handle = 1;
            Slot<> slot("null", nullptr );
            SlotPtr slotPtr;
            TEST_ASSERT( ! slotPtr.IsValid() );
            Signal<int>     signalInt;
            slotPtr.Init( *world, signalInt.GetType() );
            TEST_ASSERT( ! slotPtr.IsValid() );
            TEST_ASSERT( slotPtr.GetType() == signalInt.GetType() );
            slotPtr.Set( handle, TestComponent::Info::s_type, slot );
            TEST_ASSERT( slotPtr.IsValid() );

            // the data should move when the component is moved around in memory
            uint8_t data[ sizeof(SlotPtr) ];
            std::memcpy( data, &slotPtr, sizeof(SlotPtr) );
            SlotPtr* slotPtrCpy = (SlotPtr*)data;
            TEST_ASSERT( &slotPtrCpy->Data() == &slotPtr.Data() );
        }

        void TestSignalSlot()
        {
            EcsWorld world;
            world.AddComponentType<TestComponent>();
            EcsEntity entity = world.CreateEntity();
            world.AddComponent<TestComponent>(entity);
            EcsHandle handle = world.AddHandle( entity );

            TestComponent& testComponent = world.AddComponent<TestComponent>(entity);

            Signal<int>     signalInt;
            Slot<int> slotInt ( "test int", &TestComponent::SetValueInt );
            SlotPtr slotPtrInt;
            slotPtrInt.Init( world, signalInt.GetType() );
            slotPtrInt.Set( handle, TestComponent::Info::s_type, slotInt );

            signalInt.Connect( world, slotPtrInt );
            TEST_ASSERT( testComponent.mValueInt == 0 );
            signalInt.Emmit(12);
            TEST_ASSERT( testComponent.mValueInt == 12 );
            signalInt.Disconnect( (size_t)&slotPtrInt.Data() );
            signalInt.Emmit(25);
            TEST_ASSERT( testComponent.mValueInt == 12 );

            Signal<float>   signalFloat;
            Slot<float>slotFloat( "test float", &TestComponent::SetValueFloat );
            SlotPtr slotPtrFloat;
            slotPtrFloat.Init( world, signalFloat.GetType() );
            slotPtrFloat.Set( handle, TestComponent::Info::s_type, slotFloat );
            signalFloat.Connect( world, slotPtrFloat );
            TEST_ASSERT( testComponent.mValueFloat == 0 );
            signalFloat.Emmit(13.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );

            signalFloat.Clear();
            signalFloat.Emmit(14.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );
        }
    };
}