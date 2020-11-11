#pragma once

#include "fanUnitTest.hpp"
#include "core/ecs/fanSlot.hpp"
#include "core/ecs/fanEcsWorld.hpp"
#include "core/ecs/fanEcsComponent.hpp"
#include "core/ecs/fanEcsSingleton.hpp"

namespace fan
{
    //========================================================================================================
    //========================================================================================================
    struct TestSingleton : public EcsSingleton
    {
        ECS_SINGLETON( TestSingleton );
        static void SetInfo( EcsSingletonInfo& _info )
        {
            _info.mName = "test singleton";
            _info.mSlots.push_back( new Slot<int>( "test int", &TestSingleton::SetValueInt ) );
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
    struct TestComponent : public EcsComponent
    {
        ECS_COMPONENT( TestComponent )
        static void SetInfo( EcsComponentInfo& _info )
        {
            _info.mName = "test";
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
        static void DoNothing( EcsComponent& _component )
        {
            (void)_component;
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
            return { { &UnitTestSignal::TestConnect,            "Connect " },
                     { &UnitTestSignal::TestSlotPtrComponent,   "Slot ptr component" },
                     { &UnitTestSignal::TestSlotPtrSingleton,   "Slot ptr singleton" },
                     { &UnitTestSignal::TestSignalSlotComponent,"Signal slot component" },
                     { &UnitTestSignal::TestSignalSlotSingleton,"Signal slot singleton" },
            };
        }
        void Create() override
        {
            mWorld.AddComponentType<TestComponent>();
            mWorld.AddSingletonType<TestSingleton>();
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

        void TestSlotPtrComponent()
        {
            EcsHandle handle = 1;
            Slot<> slot("null", &TestComponent::DoNothing );
            SlotPtr slotPtr;
            TEST_ASSERT( ! slotPtr.IsValid() );
            Signal<int>     signalInt;
            slotPtr.Init( mWorld, signalInt.GetType() );
            TEST_ASSERT( ! slotPtr.IsValid() );
            TEST_ASSERT( slotPtr.GetArgsType() == signalInt.GetType() );
            slotPtr.SetComponentSlot( handle, TestComponent::Info::sType, &slot );
            TEST_ASSERT( slotPtr.IsValid() );

            // the data should move when the component is moved around in memory
            uint8_t data[ sizeof(SlotPtr) ];
            std::memcpy( data, &slotPtr, sizeof(SlotPtr) );
            SlotPtr* slotPtrCpy = (SlotPtr*)data;
            TEST_ASSERT( &slotPtrCpy->Data() == &slotPtr.Data() );
        }

        void TestSlotPtrSingleton()
        {
            Slot< int > slot("null", &TestSingleton::SetValueInt );
            SlotPtr slotPtr;
            TEST_ASSERT( ! slotPtr.IsValid() );
            Signal<int>     signalInt;
            slotPtr.Init( mWorld, signalInt.GetType() );
            TEST_ASSERT( ! slotPtr.IsValid() );
            TEST_ASSERT( slotPtr.GetArgsType() == signalInt.GetType() );
            slotPtr.SetSingletonSlot( TestSingleton::Info::sType, &slot );
            TEST_ASSERT( slotPtr.IsValid() );
        }

        void TestSignalSlotComponent()
        {
            EcsEntity entity = mWorld.CreateEntity();
            EcsHandle handle = mWorld.AddHandle( entity );
            TestComponent& testComponent = mWorld.AddComponent<TestComponent>(entity);

            // get slots
            const EcsComponentInfo componentInfo = mWorld.GetComponentInfo( TestComponent::Info::sType );
            TEST_ASSERT( componentInfo.mSlots[0]->GetArgsType() == TemplateType::Type<int>() );
            Slot<int>& slotInt = * ( (Slot<int>*)componentInfo.mSlots[0] );
            TEST_ASSERT( componentInfo.mSlots[1]->GetArgsType() == TemplateType::Type<float>() );
            Slot<float>& slotFloat = * ( (Slot<float>*)componentInfo.mSlots[1] );

            Signal<int>     signalInt;
            SlotPtr slotPtrInt;
            slotPtrInt.Init( mWorld, signalInt.GetType() );
            slotPtrInt.SetComponentSlot( handle, TestComponent::Info::sType, &slotInt );

            signalInt.Connect( mWorld, slotPtrInt );
            TEST_ASSERT( testComponent.mValueInt == 0 );
            signalInt.Emmit(12);
            TEST_ASSERT( testComponent.mValueInt == 12 );
            signalInt.Disconnect( (size_t)&slotPtrInt.Data() );
            signalInt.Emmit(25);
            TEST_ASSERT( testComponent.mValueInt == 12 );

            Signal<float>   signalFloat;
            SlotPtr slotPtrFloat;
            slotPtrFloat.Init( mWorld, signalFloat.GetType() );
            slotPtrFloat.SetComponentSlot( handle, TestComponent::Info::sType, &slotFloat );
            signalFloat.Connect( mWorld, slotPtrFloat );
            TEST_ASSERT( testComponent.mValueFloat == 0 );
            signalFloat.Emmit(13.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );

            signalFloat.Clear();
            signalFloat.Emmit(14.f);
            TEST_ASSERT( testComponent.mValueFloat == 13.f );
        }

        void TestSignalSlotSingleton()
        {
            TestSingleton& testSingleton = mWorld.GetSingleton<TestSingleton>();

            // get slot
            const EcsSingletonInfo& singletonInfo = mWorld.GetSingletonInfo( TestSingleton::Info::sType );
            TEST_ASSERT( singletonInfo.mSlots[0]->GetArgsType() == TemplateType::Type<int>() );
            Slot<int>& slotInt = * ( (Slot<int>*)singletonInfo.mSlots[0] );

            Signal<int>     signalInt;
            SlotPtr slotPtrInt;
            slotPtrInt.Init( mWorld, signalInt.GetType() );
            slotPtrInt.SetSingletonSlot( TestSingleton::Info::sType, &slotInt );

            signalInt.Connect( mWorld, slotPtrInt );
            TEST_ASSERT( testSingleton.mValueInt == 0 );
            signalInt.Emmit(12);
            TEST_ASSERT( testSingleton.mValueInt == 12 );
            signalInt.Disconnect( (size_t)&slotPtrInt.Data() );
            signalInt.Emmit(25);
            TEST_ASSERT( testSingleton.mValueInt == 12 );

            signalInt.Connect( mWorld, slotPtrInt );
            signalInt.Emmit(42);
            TEST_ASSERT( testSingleton.mValueInt == 42 );
            signalInt.Clear();
            signalInt.Emmit(64);
            TEST_ASSERT( testSingleton.mValueInt == 42 );
        }
    };
}