#pragma once

#include "core/resources/fanResourcePtr.hpp"
#include "editor/fanResourceInfos.hpp"

namespace fan
{
    struct Texture;
    struct Mesh;
    struct SkinnedMesh;
    class Prefab;
    class Font;
    struct Animation;
}

//======================================================================================================================================================================================================
//======================================================================================================================================================================================================
namespace ImGui
{
    //==================================================================================================================================================================================================
    //==================================================================================================================================================================================================
    namespace impl
    {
        bool FanResourcePtr( const char* _label, fan::ResourcePtrData& _resourcePtrData, const fan::EditorResourceInfo& _info );
    }

    template< class _ResourceType >
    bool FanResourcePtr( const char* _label, fan::ResourcePtrData& _resourcePtrData )
    {
        static_assert( std::is_base_of<fan::Resource, _ResourceType>::value );
        const fan::EditorResourceInfo& resourceInfo = fan::EditorResourceInfo::sResourceInfos.at( _ResourceType::Info::sType );
        return impl::FanResourcePtr( _label, _resourcePtrData, resourceInfo );
    }


    bool FanMeshPtr( const char* _label, fan::ResourcePtr<fan::Mesh>& _ptr );
    bool FanMeshSkinnedPtr( const char* _label, fan::ResourcePtr<fan::SkinnedMesh>& _ptr );
    bool FanPrefabPtr( const char* _label, fan::ResourcePtr<fan::Prefab>& _ptr );
    bool FanFontPtr( const char* _label, fan::ResourcePtr<fan::Font>& _ptr );
    bool FanAnimationPtr( const char* _label, fan::ResourcePtr<fan::Animation>& _ptr );
}