#pragma once

#include <string>

namespace fan
{
    struct EcsComponent;

    //========================================================================================================
    //========================================================================================================
    struct TemplateType
    {
        enum ArgsType{ int1, float1, float2, unSupported };

        template< typename... T >
        static int Type()
        {
            fanAssert( false );
            return ArgsType::unSupported;
        }
        template<> static int Type<int>()           { return ArgsType::int1;  }
        template<> static int Type<float>()         { return ArgsType::float1;}
        template<> static int Type<float, float>()  { return ArgsType::float2;}
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

        Slot( std::string _name, Method _method ) : mMethod( _method){ mName = _name; }
        int GetType() const override{ return TemplateType::Type<T...>(); }
    };
}