#pragma once

#include <string>
#include "ecs/fanEcsTypes.hpp"
#include "ecs/fanEcsComponent.hpp"
#include "ecs/fanEcsSingleton.hpp"

namespace fan
{
    class EcsWorld;

    //========================================================================================================
    //========================================================================================================
    struct TemplateType
    {
        enum ArgsType{ UnSupported = -1, Void0 = 0, Int1, Float1, Float2 };

        template< typename... T >
        static int Type()
        {
            fanAssert( false );
            return ArgsType::UnSupported;
        }
        template<> static int Type<>()              { return ArgsType::Void0;  }
        template<> static int Type<int>()           { return ArgsType::Int1;  }
        template<> static int Type<float>()         { return ArgsType::Float1;}
        template<> static int Type<float, float>()  { return ArgsType::Float2;}
    };

    //========================================================================================================
    //========================================================================================================
    struct SlotBase
    {
        virtual int GetArgsType() const = 0;
        enum TargetType{ Singleton, Component, Invalid };

        std::string mName;
        TargetType  mTargetType = TargetType::Invalid;
    };

    //========================================================================================================
    //========================================================================================================
    template < typename... T >
    struct Slot : public SlotBase
    {

        using MethodComponent = void(*)( EcsComponent&, T... );
        using MethodSingleton = void(*)( EcsSingleton&, T... );
        union
        {
            MethodComponent mMethodComponent;
            MethodSingleton mMethodSingleton;
        }
        mMethod;

        Slot( std::string _name, MethodComponent _method )
        {
            mName = _name;
            mMethod.mMethodComponent = _method;
            mTargetType = TargetType::Component;
        }

        Slot( std::string _name, MethodSingleton _method )
        {
            mName = _name;
            mMethod.mMethodSingleton = _method;
            mTargetType = TargetType::Singleton;
        }

        int GetArgsType() const override{ return TemplateType::Type<T...>(); }
    };

    //========================================================================================================
    //========================================================================================================
    class SlotPtr
    {
    public:
        struct SlotCallData
        {
            EcsHandle mHandle = 0;
            uint32_t  mType = 0; // componentType or singletonType
            SlotBase* mSlot = nullptr;
        };

        SlotPtr();
        ~SlotPtr();
        bool IsValid() const { return mCallData->mType != 0; }
        void Clear();
        void Init( EcsWorld& _world, int _argsType );
        void SetComponentSlot( EcsHandle _handle, uint32_t _componentType, SlotBase* _slot );
        void SetComponentSlot( EcsHandle _handle, uint32_t _componentType, const std::string& _slotName );
        void SetSingletonSlot( uint32_t _singletonType, SlotBase* _slot );
        void SetSingletonSlot( uint32_t _singletonType, const std::string& _slotName );
        int  GetArgsType() const { return mArgsType; }
        const SlotCallData& Data() const { return *mCallData; }
        EcsWorld& World() const { return *mWorld; }
        bool IsSingletonSlot() const { return IsValid() && mCallData->mHandle == 0; }
        bool IsComponentSlot() const { return IsValid() && mCallData->mHandle != 0; }
    private:
        int mArgsType = TemplateType::UnSupported;
        SlotCallData* mCallData = nullptr;
        EcsWorld    * mWorld    = nullptr;
    };


}

namespace ImGui
{
    void FanSlotPtr( const char* _label, fan::EcsWorld& _world, fan::SlotPtr& _ptr );
}