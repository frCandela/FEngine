#pragma once

#include "engine/fanSceneResourcePtr.hpp"

//============================================================================================================
// ImGui bindings
//============================================================================================================
namespace ImGui
{
    bool FanPrefab( const char* _label, fan::PrefabPtr& _ptr );
    bool FanComponentBase( const char* _label, fan::ComponentPtrBase& _ptr );
    template< typename _componentType >
    bool FanComponent( const char* _label, fan::ComponentPtr<_componentType>& _ptr )
    {
        static_assert( ( std::is_base_of<fan::EcsComponent, _componentType>::value ) );
        return FanComponentBase( _label, _ptr );
    }
}