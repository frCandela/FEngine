#pragma once

#include "core/resources/fanResourcePtr.hpp"

namespace fan
{
    struct Texture;
    struct Mesh;
    struct MeshSkinned;
    class Prefab;
    class Font;
}

//============================================================================================================
// ImGui widgets
//============================================================================================================
namespace ImGui
{
    bool FanTexturePtr( const char* _label, fan::ResourcePtr<fan::Texture>& _ptr );
    bool FanMeshPtr( const char* _label, fan::ResourcePtr<fan::Mesh>& _ptr );
    bool FanMeshSkinnedPtr( const char* _label, fan::ResourcePtr<fan::MeshSkinned>& _ptr );
    bool FanPrefabPtr( const char* _label, fan::ResourcePtr<fan::Prefab>& _ptr );
    bool FanFontPtr( const char* _label, fan::ResourcePtr<fan::Font>& _ptr );
}