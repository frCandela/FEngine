#pragma once

#include <string>
#include "ecs/fanEcsTypes.hpp"

namespace fan
{
    struct EcsComponent;
    class EcsWorld;

    //========================================================================================================
    //========================================================================================================
    struct TemplateType
    {
        enum ArgsType{ Void0, Int1, Float1, Float2, UnSupported };

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
        virtual int GetType() const = 0;
        std::string mName;
    };

    //========================================================================================================
    //========================================================================================================
    template < typename... T >
    struct Slot : public SlotBase
    {
        using Method = void(*)( EcsComponent&, T... );
        Method  mMethod;

        Slot( std::string _name, Method _method ) : mMethod( _method)
        {
            mName = _name;
        }
        int GetType() const override{ return TemplateType::Type<T...>(); }
    };

    //========================================================================================================
    //========================================================================================================
    struct SlotPtr
    {
        struct SlotCallData
        {
            EcsHandle mHandle        = 0;
            uint32_t  mComponentType = 0;
            SlotBase* mSlot = nullptr;
        };

        SlotPtr();
        ~SlotPtr();
        bool IsValid() const;
        void Clear();
        void Init( EcsWorld& _world, int _argsType );
        void Set( EcsHandle _handle, uint32_t _componentType, SlotBase& _slot );
        int GetType() const { return mArgsType; }
        const SlotCallData& Data() const { return *mCallData; }

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