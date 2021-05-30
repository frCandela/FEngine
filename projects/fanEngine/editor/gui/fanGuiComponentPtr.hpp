#pragma once

#include "engine/resources/fanComponentPtr.hpp"

//============================================================================================================
// ImGui bindings
//============================================================================================================
namespace ImGui
{
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
    template< typename _componentType >
    bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
    {
        static_assert( ( std::is_base_of<fan::EcsComponent, _componentType>::value ) );
        return FanComponentBase( _label, _ptr );
    }
}