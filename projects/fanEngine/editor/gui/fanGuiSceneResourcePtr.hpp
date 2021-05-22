#pragma once

#include "engine/resources/fanSceneResourcePtr.hpp"

namespace fan
{
    class Prefab;
    class Font;
}

//============================================================================================================
// ImGui bindings
//============================================================================================================
namespace ImGui
{
    bool FanPrefab( const char* _label, fan::ResourcePtr<fan::Prefab>& _ptr );
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
    template< typename _componentType >
    bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
    {
        static_assert( ( std::is_base_of<fan::EcsComponent, _componentType>::value ) );
        return FanComponentBase( _label, _ptr );
    }
    bool FanFont( const char* _label, fan::ResourcePtr<fan::Font>& _ptr );
}